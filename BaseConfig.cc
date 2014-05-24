/*!
 * \file BaseConfig.cc
 * \brief Implementation of BaseConfig class.
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

#include <fstream>
#include "exception.h"
#include "BaseConfig.h"

void vsc::BaseConfig::Read(const std::string& fileName)
{
    std::ifstream f(fileName.c_str());
    if(!f.is_open())
        THROW_VSC_EXCEPTION("Unable to read the configuration file '" << fileName << "'.");
    while(f.good()) {
        std::string line;
        std::getline(f, line);
        if(!line.length() || line[0] == '#' || line[0] == '-')
            continue;

        std::istringstream istring( line);
        std::string name;
        std::string value;

        istring >> name >> value;

        if(istring.fail() || !name.length())
            continue;
        parameters[name] = value;
    }
}

void vsc::BaseConfig::Write(const std::string& fileName) const
{
    std::ofstream f(fileName.c_str());
    if(!f.is_open())
        THROW_VSC_EXCEPTION("Unable to write the configuration into the file '" << fileName << "'.");
    for(Map::const_iterator iter = parameters.begin(); iter != parameters.end(); ++iter) {
        f << iter->first << " " << iter->second << std::endl;
    }
}
