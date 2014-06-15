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
#define THROW_VSC_EXCEPTION(short_msg, msg) { \
        std::stringstream ss, ss_short; \
        ss << msg; ss_short << short_msg; \
        throw vsc::exception(__PRETTY_FUNCTION__, ss_short.str(), ss.str()); }
#else
#define THROW_VSC_EXCEPTION(short_msg, msg) { \
        std::stringstream ss, ss_short; \
        ss << msg; ss_short << short_msg; \
        throw vsc::exception(__FUNCTION__, ss_short.str(), ss.str()); }
#endif

namespace vsc {
class exception : public std::exception {
public:
    explicit exception() {}
    exception(const std::string& header, const std::string& short_message, const std::string& message)
        : hdr(header), short_msg(short_message), msg(message), full_msg("[" + hdr + "] " + msg) {}
    virtual ~exception() throw() {}
    virtual const char* what() const throw() { return full_msg.c_str(); }
    const std::string& header() const { return hdr; }
    const std::string& short_message() const {  return short_msg; }
    const std::string& message() const { return msg; }
    const std::string& full_message() const { return full_msg; }
private:
    std::string hdr;
    std::string short_msg;
    std::string msg;
    std::string full_msg;
};
} // vsc
