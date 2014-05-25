/*!
 * \file VoltageSourceFactory.cc
 * \brief Implementation of VoltageSourceFactory class.
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

#include <map>

#include "ConfigParameters.h"
#include "Keithley237.h"
#include "Keithley6487.h"
#include "VoltageSourceFactory.h"
#include "FakeVoltageSource.h"

typedef vsc::IVoltageSource* (*Maker)(const ConfigParameters&);
typedef std::map<std::string, Maker> MakerMap;

//static vsc::IVoltageSource* Keithley237Maker(const ConfigParameters& configParameters)
//{
//    const vsc::Keithley237::Configuration keithleyConfig(configParameters.VoltageSourceDevice(),
//            configParameters.SetVoltageSourceToLocalModeOnExit(),
//            configParameters.NumberOfVoltageSourceReadingsToAverage(),
//            configParameters.VoltageSourceIntegrationTime());
//    return new vsc::Keithley237(keithleyConfig);
//}

static vsc::IVoltageSource* FakeVoltageSourceMaker(const ConfigParameters&)
{
    return new vsc::FakeVoltageSource(100.0 * vsc::mega * vsc::ohms, 5.0 * vsc::seconds, true);
}

static MakerMap CreateMakerMap()
{
    MakerMap map;
//    map["Keithley237"] = &Keithley237Maker;
    map["Fake"] = &FakeVoltageSourceMaker;
    return map;
}

static const MakerMap makerMap = CreateMakerMap();

static vsc::IVoltageSource* CreateVoltageSource()
{
    const ConfigParameters& configParameters = ConfigParameters::Singleton();
    MakerMap::const_iterator iter = makerMap.find(configParameters.VoltageSource());
    if(iter == makerMap.end())
        THROW_VSC_EXCEPTION("Configuration error", "Voltage source '" << configParameters.VoltageSource()
                            << "' not found.");
    return iter->second(configParameters);
}

vsc::VoltageSourceFactory::Pointer vsc::VoltageSourceFactory::Get()
{
    static Pointer voltageSource(new ThreadSafeVoltageSource(CreateVoltageSource()));
    return voltageSource;
}

const vsc::VoltageSourceFactory::NameSet& vsc::VoltageSourceFactory::GetNames()
{
    static NameSet voltageSources;
    if(!voltageSources.size()) {
        voltageSources.insert("Fake");
    }
    return voltageSources;
}
