/*!
 * \file ThreadSafeVoltageSource.cc
 * \brief Implementation of ThreadSafeVoltageSource class.
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

#include "ThreadSafeVoltageSource.h"
#include "exception.h"
#include "date_time.h"
#include "log.h"

vsc::ThreadSafeVoltageSource::ThreadSafeVoltageSource(IVoltageSource* aVoltageSource, bool _saveMeasurements)
    : voltageSource(aVoltageSource), saveMeasurements(_saveMeasurements), isOn(false)
{
    if(!aVoltageSource)
        THROW_VSC_EXCEPTION("Ivalid parameters", "Voltage source can't be null.");
}

vsc::IVoltageSource::Value vsc::ThreadSafeVoltageSource::Set(const Value& value)
{
    const std::lock_guard<std::recursive_mutex> lock(mutex);
    if(!isOn || currentValue != value) {
        currentValue = voltageSource->Set(value);
        isOn = true;
    }
    return currentValue;
}

vsc::ElectricPotential vsc::ThreadSafeVoltageSource::Accuracy(const vsc::ElectricPotential& voltage)
{
    const std::lock_guard<std::recursive_mutex> lock(mutex);
    return voltageSource->Accuracy(voltage);
}

vsc::IVoltageSource::Measurement vsc::ThreadSafeVoltageSource::Measure()
{
    const std::lock_guard<std::recursive_mutex> lock(mutex);
    const IVoltageSource::Measurement measurement = voltageSource->Measure();
    if(saveMeasurements)
        measurements.push_back(measurement);
    if(onMeasurement)
        onMeasurement(measurement);
    return measurement;
}

bool vsc::ThreadSafeVoltageSource::GradualSet(const Value& value, const vsc::ElectricPotential& step,
        const vsc::Time& delayBetweenSteps, bool checkForCompliance)
{
    if(step <= 0.0 * vsc::volts)
        THROW_VSC_EXCEPTION("Invalid parameters", "Invalid voltage step = " << step << ". The voltage step should be"
                            " greater then zero.");
    if(delayBetweenSteps < 0.0 * vsc::seconds)
        THROW_VSC_EXCEPTION("Invalid parameters", "Invalid delay between the voltage switch = " << delayBetweenSteps
                            << ". The delay should be positive or zero.");

    const std::lock_guard<std::recursive_mutex> lock(mutex);
    for(bool makeNextStep = true; makeNextStep;) {
        const vsc::ElectricPotential deltaV = value.Voltage - currentValue.Voltage;
        const vsc::ElectricPotential absDeltaV = vsc::abs(deltaV);
        vsc::ElectricPotential voltageToSet;
        if(absDeltaV < Accuracy(value.Voltage))
            break;
        if(absDeltaV < vsc::abs(step)) {
            makeNextStep = false;
            voltageToSet = value.Voltage;
        } else {
            voltageToSet = currentValue.Voltage + (deltaV > 0.0 * vsc::volts ? step : -step);
        }

        Set(Value(voltageToSet, value.Compliance));
        vsc::Sleep(delayBetweenSteps);

        if(checkForCompliance) {
            const Measurement measurement = Measure();
            if(measurement.Compliance)
                return false;
        }
    }
    return true;
}

void vsc::ThreadSafeVoltageSource::Off()
{
    const std::lock_guard<std::recursive_mutex> lock(mutex);
    voltageSource->Off();
    currentValue.Voltage = 0.0 * vsc::volts;
    isOn = false;
}

void vsc::ThreadSafeVoltageSource::lock()
{
    mutex.lock();
}

void vsc::ThreadSafeVoltageSource::unlock()
{
    mutex.unlock();
}
