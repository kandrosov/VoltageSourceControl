/*!
 * \file VoltageSourceFactory.h
 * \brief Definition of VoltageSourceFactory class.
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

#include <set>
#include <boost/shared_ptr.hpp>
#include "ThreadSafeVoltageSource.h"

namespace vsc {
class VoltageSourceFactory {
public:
    typedef boost::shared_ptr<ThreadSafeVoltageSource> Pointer;
    typedef std::set<std::string> NameSet;
    static Pointer Get();
    static const NameSet& GetNames();

private:
    VoltageSourceFactory() {}
};

}
