/*!
 * \file BaseConfig.h
 * \brief Definition of BaseConfig class.
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

#include <map>
#include "log.h"
#include "units.h"

#define VSC_CONFIG_NAME(name) \
    static const std::string& ConfigName() \
    { \
        static const std::string config_name = name; \
        return config_name; \
    }


#define VSC_CONFIG_PARAMETER(type, name, default_value) \
    type name() const { \
        type result = default_value; \
        if(!Get(#name, result)) \
            vsc::LogInfo(ConfigName()) << "Warning: Parameter '" << #name \
                                       << "' is not set. Using default value = '" << default_value << "'.\n"; \
        return result; \
    } \
    void set##name(const type& value) { Set(#name, value); }

namespace vsc {

namespace BaseConfigInternals {

template<typename Value>
struct ConfigValue {
    static bool Read(const std::string& str, Value& value) {
        std::istringstream s(str);
        s >> value;
        return !s.fail();
    }
};

template<>
struct ConfigValue<vsc::ElectricCurrent> {
    static const vsc::ElectricCurrent& UnitsFactor() {
        static const vsc::ElectricCurrent factor = 1.0 * vsc::amperes;
        return factor;
    }

    static bool Read(const std::string& str, vsc::ElectricCurrent& value) {
        double v;
        if(!ConfigValue<double>::Read(str, v))
            return false;
        value = v * UnitsFactor();
        return true;
    }
};

template<>
struct ConfigValue<vsc::ElectricPotential> {
    static const vsc::ElectricPotential& UnitsFactor() {
        static const vsc::ElectricPotential factor = 1.0 * vsc::volts;
        return factor;
    }

    static bool Read(const std::string& str, vsc::ElectricPotential& value) {
        double v;
        if(!ConfigValue<double>::Read(str, v))
            return false;
        value = v * UnitsFactor();
        return true;
    }
};

template<>
struct ConfigValue<vsc::Time> {
    static const vsc::Time& UnitsFactor() {
        static const vsc::Time factor = 1.0 * vsc::seconds;
        return factor;
    }

    static bool Read(const std::string& str, vsc::Time& value) {
        double v;
        if(!ConfigValue<double>::Read(str, v))
            return false;
        value = v * UnitsFactor();
        return true;
    }
};

}

class BaseConfig {
private:
    typedef std::map<std::string, std::string> Map;
public:
    virtual ~BaseConfig() {}
    virtual void Read(const std::string& fileName);
    virtual void Write(const std::string& fileName) const;

protected:
    template<typename Value>
    bool Get(const std::string& name, Value& value) const {
        const Map::const_iterator iter = parameters.find(name);
        if(iter == parameters.end())
            return false;
        return BaseConfigInternals::ConfigValue<Value>::Read(iter->second, value);
    }

    template<typename Value>
    void Set(const std::string& name, const Value& value) {
        std::ostringstream s;
        s << value;
        parameters[name] = s.str();
    }

private:
    Map parameters;
};

}
