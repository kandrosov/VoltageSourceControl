/*!
 * \file Keithley6487.cc
 * \brief Implementation of Keithley6487 class.
 * \author Konstantin Androsov (INFN Pisa, Siena University)
 *
 * Copyright 2013, 2014 Konstantin Androsov <konstantin.androsov@gmail.com>
 *
 * This file is part of VoltageSourceControl.
 *
 * VoltageSourceControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * VoltageSourceControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VoltageSourceControl.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Keithley6487.h"
#include "exception.h"
#include "date_time.h"

static const unsigned DEFAULT_TIMEOUT = 3;
static const unsigned OPERATION_IS_COMPLETE_INDICATOR = 1;
static const unsigned MAX_VOLTAGE_RANGE = 500; // V
static const std::string MAX_CURRENT_LIMIT = "2.5e-3"; // A
static const std::string IDENTIFICATION_STRING_PREFIX = "KEITHLEY INSTRUMENTS INC.,MODEL 6487";

static const vsc::ElectricPotential VOLTAGE_FACTOR = 1.0 * vsc::volts;
static const vsc::ElectricCurrent CURRENT_FACTOR = 1.0 * vsc::amperes;

const double vsc::Keithley6487::MAX_VOLTAGE = 500;
const vsc::ElectricPotential vsc::Keithley6487::ACCURACY = 0.1 * vsc::volts;


vsc::Keithley6487::Keithley6487(const std::string& deviceName, unsigned baudrate,
                                SerialOptions::FlowControl flowControl, SerialOptions::Parity parity,
                                unsigned char characterSize)
{
    SerialOptions options;
    options.setDevice(deviceName);
    options.setTimeout(boost::posix_time::seconds(DEFAULT_TIMEOUT));

    options.setBaudrate(baudrate);
    options.setFlowControl(flowControl);
    options.setParity(parity);
    options.setCsize(characterSize);

    try {
        serialStream = boost::shared_ptr<SerialStream>(new SerialStream(options));
        serialStream->exceptions(std::ios::badbit | std::ios::failbit);
        Send("*RST");
        Send("*IDN?");
        const std::string identificationString = ReadString();
        if(identificationString.find(IDENTIFICATION_STRING_PREFIX) != 0)
            THROW_VSC_EXCEPTION("Connection error", "Device connected to '" << deviceName << "' is not supported."
                                " Device identified it self as '" << identificationString << "'.");
        Send("FUNC 'CURR'");
        Send("FORM:ELEM READ,VSO");
    } catch(std::ios_base::failure&) {
        THROW_VSC_EXCEPTION("Connection error", "Unable to connect to the Keithley on '" << deviceName << "'.");
    }
}

vsc::Keithley6487::~Keithley6487()
{
    Send("*RST");
    Send("SYST:LOC");
}

vsc::IVoltageSource::Value vsc::Keithley6487::Set(const Value& value)
{
    const double voltage = value.Voltage / VOLTAGE_FACTOR;
    if(voltage > MAX_VOLTAGE)
        THROW_VSC_EXCEPTION("Invalid parameters", "Voltage " << voltage << " V is too high."
                            << "Maximal allowed voltage is " << MAX_VOLTAGE << " V.");

    try {
        Send("SOUR:VOLT:RANG", MAX_VOLTAGE_RANGE);
        Send("SOUR:VOLT", voltage);
        Send("SOUR:VOLT:ILIM", MAX_CURRENT_LIMIT);
        Send("SOUR:VOLT:STAT ON");

        if(!LastOperationIsCompleted())
            THROW_VSC_EXCEPTION("Error on device", "Voltage was not set.");
        return value;
    } catch(TimeoutException&) {
        THROW_VSC_EXCEPTION("Connection error", "Unable to connect to the Keithley to set a voltage.");
    } catch(std::ios_base::failure&) {
        THROW_VSC_EXCEPTION("Connection error", "Unable to connect to the Keithley to set a voltage.");
    }
}

vsc::ElectricPotential vsc::Keithley6487::Accuracy(const vsc::ElectricPotential&)
{
    return ACCURACY;
}

vsc::IVoltageSource::Measurement vsc::Keithley6487::Measure()
{
    Send("READ?");
    const Measurement m = Read<Measurement>();
    return IVoltageSource::Measurement(m.Current, m.Voltage, DateTimeProvider::ElapsedTime(), m.Compliance);
}

void vsc::Keithley6487::Off()
{
    Send("SOUR:VOLT:STAT OFF");
    if(!LastOperationIsCompleted())
        THROW_VSC_EXCEPTION("Error on device", "Voltage is not turned off.");
}

std::string vsc::Keithley6487::ReadString()
{
    std::string s;
    std::getline(*serialStream, s);
    return s;
}

bool vsc::Keithley6487::LastOperationIsCompleted()
{
    Send("*OPC?");
    const unsigned operationStatus = Read<unsigned>();
    return operationStatus == OPERATION_IS_COMPLETE_INDICATOR;
}

std::istream& operator >>(std::istream& s, vsc::Keithley6487::Measurement& m)
{
    char c;
    double current;
    s >> current;
    m.Current = current * CURRENT_FACTOR;
    s >> c;
    if(c != ',')
        THROW_VSC_EXCEPTION("Connection error", "Keithley replay has an incorrect format.");
    double voltage;
    s >> voltage;
    m.Voltage = voltage * VOLTAGE_FACTOR;
    m.Compliance = false;
    return s;
}
