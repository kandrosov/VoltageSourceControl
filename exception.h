/*!
 * \file exception.h
 * \brief Definition of the base exception class for the vsc namespace.
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

#include <exception>
#include <string>
#include <sstream>

#ifdef __GNUG__
#define THROW_VSC_EXCEPTION(msg)   {    \
        std::stringstream ss;               \
        ss << msg;                          \
        throw vsc::exception(__PRETTY_FUNCTION__, ss.str()); }
#else
#define THROW_VSC_EXCEPTION(msg)   {    \
        std::stringstream ss;               \
        ss << msg;                          \
        throw vsc::exception(__FUNCTION__, ss.str()); }

#endif

namespace vsc {
class exception : public std::exception {
public:
    exception(const std::string& header, const std::string& message) : hdr(header), msg(message) {}
    virtual ~exception() throw() {}
    virtual const char* what() const throw() {
        return ("[" + hdr + "] " + msg).c_str();
    }
    const std::string& header() const {
        return hdr;
    }
    const std::string& message() const {
        return msg;
    }
private:
    std::string hdr;
    std::string msg;
};
} // vsc
