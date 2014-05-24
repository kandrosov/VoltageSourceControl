/*!
 * \file Keithley237.cc
 * \brief Implementation of Keithley237 class.
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

#ifdef GPIB_SUPPORT

#include "Keithley237.h"
#include "date_time.h"

using namespace vsc::Keithley237Internals;
using namespace vsc::Keithley237Internals::Commands;

const vsc::ElectricCurrent vsc::Keithley237::MAX_COMPLIANCE = 0.01 * vsc::amperes;
const vsc::ElectricPotential vsc::Keithley237::ACCURACY = 0.1 * vsc::volts;


vsc::Keithley237::Keithley237(const Configuration& configuration)
{
    try {
        gpibStream = boost::shared_ptr<GpibStream>(new GpibStream(configuration.GetDeviceName(),
                     configuration.GoLocalOnDestruction()));
        gpibStream->exceptions(std::ios::badbit | std::ios::failbit);
        Prepare();
        SendAndCheck(CmdSelfTests()(RestoreFactoryDefaults));
        SendAndCheck(CmdSetFilter()(configuration.GetFilterMode()));
        SendAndCheck(CmdSetIntegrationTime()(configuration.GetIntegrationTimeMode()));
        SendAndCheck(CmdSetOutputDataFormat()(MachineStatus::OutputDataFormat::SourceValue |
                                              MachineStatus::OutputDataFormat::MeasureValue,
                                              MachineStatus::OutputDataFormat::ASCII_Prefix_NoSuffix,
                                              MachineStatus::OutputDataFormat::OneLineFromDCBuffer));
    } catch(std::ios_base::failure& e) {
        THROW_VSC_EXCEPTION("Unable to connect to the device '" << configuration.GetDeviceName() << "'. " << std::endl
                            << e.what());
    }
}

vsc::Keithley237::~Keithley237()
{
    Off();
}

void vsc::Keithley237::Prepare()
{
    try {
        (*gpibStream) << CmdExecute()();
        gpibStream->flush();
    } catch(std::ios_base::failure&) {
        gpibStream->clear();
        gpibStream->flush();
    }
}

vsc::IVoltageSource::Value vsc::Keithley237::Set(const Value& value)
{
    if(vsc::abs(value.Voltage) > VoltageRanges.GetLastValue())
        THROW_VSC_EXCEPTION("Voltage value is out of range. Requested voltage value to set is " << value.Voltage
                            << ". Maximal supported absolut value is " << VoltageRanges.GetLastValue() << ".");
    if(vsc::abs(value.Compliance) > MAX_COMPLIANCE)
        THROW_VSC_EXCEPTION("Compliance value is out of range. Requested compliance value to set is "
                            << value.Compliance << ". Maximal supported absolut value is " << MAX_COMPLIANCE << ".");
    SendAndCheck(CmdSetSourceAndFunction()(SourceVoltageMode, DCFunction));
    SendAndCheck(CmdSetCompliance()(value.Compliance, CurrentRanges.GetAutorangeModeId()));
    SendAndCheck(CmdSetBias()(value.Voltage, VoltageRanges.GetLastMode(), 0));
    SendAndCheck(CmdSetInstrumentMode()(MachineStatus::OperateMode));
    SendAndCheck(CmdImmediateBusTrigger()());
    Send(CmdSendStatus()(SendMachineStatusWord));
    const MachineStatus machineStatus = Read<MachineStatus>();
    if(machineStatus.operate != MachineStatus::OperateMode)
        THROW_VSC_EXCEPTION("Unable to set a voltage = " << value.Voltage << " and compliance =" << value.Compliance
                            << ". After execution of all required commands Keithley is still not in the Operate Mode.");
    Send(CmdSendStatus()(SendComplianceValue));
    ComplianceValue compliance = Read<ComplianceValue>();
    IVoltageSource::Measurement measurement = Measure();

    return Value(measurement.Voltage, compliance.CurrentCompliance);
}

vsc::ElectricPotential vsc::Keithley237::Accuracy(const vsc::ElectricPotential&)
{
    return ACCURACY;
}

vsc::IVoltageSource::Measurement vsc::Keithley237::Measure()
{
    Keithley237Internals::Measurement m = Read<Keithley237Internals::Measurement>();
    return IVoltageSource::Measurement(m.Current, m.Voltage, DateTimeProvider::ElapsedTime(), m.Compliance);
}

void vsc::Keithley237::Off()
{
    SendAndCheck(CmdSetInstrumentMode()(MachineStatus::StandbyMode));
}

void vsc::Keithley237::Send(const std::string& command, bool execute)
{
    try {
        (*gpibStream) << command;
        if(execute)
            (*gpibStream) << CmdExecute()();
        gpibStream->flush();
    } catch(std::ios_base::failure& e) {
        THROW_VSC_EXCEPTION("Unable to send a command to the Keithley. Command = '" << command << "'. " << std::endl
                            << e.what() << std::endl << GpibDevice::GetReportMessage());
    }
}

void vsc::Keithley237::SendAndCheck(const std::string& command)
{
    static const std::string ERROR_MESSAGE_FORMAT = "Keithley reported %1% after executing the last command ="
            " '%2%'.\n%3%";
    Send(command, true);
    Send(CmdSendStatus()(SendErrorStatus));
    const ErrorStatus errorStatus = Read<ErrorStatus>();
    if(errorStatus.HasErrors())
        THROW_VSC_EXCEPTION(boost::format(ERROR_MESSAGE_FORMAT) % "an error" % command % errorStatus.GetErrorMessage());
    Send(CmdSendStatus()(SendWarningStatus));
    const WarningStatus warningStatus = Read<WarningStatus>();
    if(warningStatus.HasWarnings())
        THROW_VSC_EXCEPTION(boost::format(ERROR_MESSAGE_FORMAT) % "a warning" % command
                            % warningStatus.GetWarningMessage());
}

std::string vsc::Keithley237::ReadString()
{
    try {
        std::string str;
        (*gpibStream) >> str;
        return str;
    } catch(std::ios_base::failure& e) {
        THROW_VSC_EXCEPTION("Unable to read a data from the Keithley. " << std::endl << e.what());
    }
}

static Range<unsigned>::ValueRangeMap CreateFilterModes()
{
    typedef Range<unsigned>::ValueRangeMap Map;
    Map m;
    for(Map::left_key_type n = 0; n < 6; ++n)
        m.insert(Map::value_type(n, 1 << n));
    return m;
}
const Range<unsigned> vsc::Keithley237::Configuration::FilterModes(CreateFilterModes(), 1, "Filter",
        "readings to average");

static Range<vsc::Time, unsigned, double>::ValueRangeMap CreateIntegrationTimeModes()
{
    typedef Range<vsc::Time, unsigned, double>::ValueRangeMap Map;
    Map m;
    m.insert(Map::value_type(0, 416));
    m.insert(Map::value_type(1, 4000));
    m.insert(Map::value_type(2, 16670));
    m.insert(Map::value_type(3, 20000));
    return m;
}
const Range<vsc::Time, unsigned, double> vsc::Keithley237::Configuration::IntegrationTimeModes
(CreateIntegrationTimeModes(), 1e-6 * vsc::seconds, "Integration Time", "interval");

vsc::Keithley237::Configuration::Configuration(const std::string& _deviceName, bool _goLocalOnDestruction,
        unsigned numberOfReadingsToAverage, vsc::Time integrationTime)
    : deviceName(_deviceName), goLocalOnDestruction(_goLocalOnDestruction),
      filterMode(FilterModes.FindMode(numberOfReadingsToAverage)),
      integrationTimeMode(IntegrationTimeModes.FindMode(integrationTime)) {}

#endif  // GPIB_SUPPORT
