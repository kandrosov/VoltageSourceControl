/*!
 * \file ConfigParameters.h
 * \brief Definition of ConfigParameters class.
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

#include "BaseConfig.h"

#define VSC_FULL_CONFIG_FILE_NAME(name) \
    std::string Full##name() const { return FullFileName(name()); }

/*!
 * \brief Configuration parameters
 */
class ConfigParameters : public vsc::BaseConfig {
public:
    VSC_CONFIG_PARAMETER(std::string, Directory, ".")
    VSC_CONFIG_PARAMETER(std::string, ErrorFileName, "error.log")
    VSC_FULL_CONFIG_FILE_NAME(ErrorFileName)
    VSC_CONFIG_PARAMETER(std::string, DebugFileName, "debug.log")
    VSC_FULL_CONFIG_FILE_NAME(DebugFileName)
    VSC_CONFIG_PARAMETER(std::string, LogFileName, "info.log")
    VSC_FULL_CONFIG_FILE_NAME(LogFileName)

    VSC_CONFIG_PARAMETER(std::string, VoltageSource, "Keithley237")
    VSC_CONFIG_PARAMETER(std::string, VoltageSourceDevice, "keithley")
    VSC_CONFIG_PARAMETER(bool, SetVoltageSourceToLocalModeOnExit, true)
    VSC_CONFIG_PARAMETER(unsigned, NumberOfVoltageSourceReadingsToAverage, 4)
    VSC_CONFIG_PARAMETER(vsc::Time, VoltageSourceIntegrationTime, 16.670e-3 * vsc::seconds)

public:
    static ConfigParameters& ModifiableSingleton() {
        static ConfigParameters instance;
        return instance;
    }

    static const ConfigParameters& Singleton() {
        return ModifiableSingleton();
    }

public:
    void ReadConfigParameterFile() {
        Read(FullFileName(fileName));
    }

    void WriteConfigParameterFile() const {
        Write(FullFileName(fileName));
    }

private:
    std::string FullFileName(const std::string& fileName) const {
        return Directory() + "/" + fileName;
    }
    VSC_CONFIG_NAME("ConfigParameters")
    ConfigParameters() : fileName("parameters.cfg") {}
    std::string fileName;
};
