/*!
 * \file FakeVoltageSource.h
 * \brief Definition of FakeVoltageSource class.
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

#pragma once

#include "IVoltageSource.h"
#include "log.h"
#include "date_time.h"
#include "exception.h"

namespace vsc {

class FakeVoltageSource : public IVoltageSource {
public:
    FakeVoltageSource(const Resistance& resistance, const Time& _connectionDelay, bool generateConnectError,
                      bool _generateDisconnectError, bool _generateMesureError)
        : r(resistance), v(0.0 * volts), connectionDelay(_connectionDelay),
          generateDisconnectError(_generateDisconnectError), generateMeasureError(_generateMesureError)
    {
        Sleep(connectionDelay);
        if(generateConnectError)
            THROW_VSC_EXCEPTION("Connection error", "Fake connection error.");
        LogDebug("FakeVoltageSource") << "Voltage source created.";
    }

    virtual ~FakeVoltageSource()
    {
        LogDebug("FakeVoltageSource") << "Voltage source destroyed.";
    }

    /// \copydoc IVoltageSource::Set
    virtual Value Set(const Value& value) {
        LogDebug("FakeVoltageSource") << "Set value: " << value << "." << std::endl;
        v = value.Voltage;
        compliance = value.Compliance;
        return value;
    }

    /// \copydoc IVoltageSource::Accuracy
    virtual ElectricPotential Accuracy(const ElectricPotential& voltage) {
        static const ElectricPotential accuracy = 0.1 * volts;
        LogDebug("FakeVoltageSource") << "Return accuracy = " << accuracy << " for " << voltage << "." << std::endl;
        return accuracy;
    }

    /// \copydoc IVoltageSource::Measure
    virtual IVoltageSource::Measurement Measure() {
        if(generateMeasureError)
            THROW_VSC_EXCEPTION("Communication error", "Fake measure error.");
        const ElectricCurrent i = v / r;
        const bool inCompliance = i >= compliance;
        const IVoltageSource::Measurement measurement(i, v, DateTimeProvider::ElapsedTime(), inCompliance);
        LogDebug("FakeVoltageSource") << "Return measurement: " << measurement << "." << std::endl;
        return measurement;
    }

    /// \copydoc IVoltageSource::Off
    virtual void Off() {
        Sleep(connectionDelay);
        if(generateDisconnectError)
            THROW_VSC_EXCEPTION("Connection error", "Fake disconnection error.");

        LogDebug("FakeVoltageSource") << "Turn off." << std::endl;
    }

private:
    vsc::Resistance r;
    vsc::ElectricPotential v;
    vsc::ElectricCurrent compliance;
    vsc::Time connectionDelay;
    bool generateDisconnectError, generateMeasureError;
};

} // vsc
