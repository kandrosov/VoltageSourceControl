/*!
 * \file date_time.cc
 * \brief Implementation of the time-related functionality for the vsc namespace.
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

#include <thread>
#include <boost/date_time.hpp>

#include "date_time.h"

namespace {
boost::posix_time::ptime _Now()
{
    return boost::date_time::microsec_clock<boost::posix_time::ptime>::universal_time();
}

const boost::posix_time::ptime& _StartTime()
{
    static boost::posix_time::ptime startTime = _Now();
    return startTime;
}
}

namespace vsc {

std::chrono::microseconds TimeToChronoDuration(const Time& time)
{
    static const Time time_factor = 1.0 * micro * seconds;
    const double delay_in_micro_seconds = time / time_factor;
    return std::chrono::microseconds(static_cast<int64_t>(delay_in_micro_seconds));
}

void Sleep(const Time& time)
{
    const std::chrono::microseconds delay = TimeToChronoDuration(time);
    std::this_thread::sleep_for(delay);
}

std::string DateTimeProvider::Now()
{
    const boost::posix_time::ptime now = _Now();
    return boost::posix_time::to_iso_extended_string(now);
}

std::string DateTimeProvider::TimeNow()
{
    const boost::posix_time::ptime now = _Now();
    return boost::posix_time::to_simple_string(now.time_of_day());
}

std::string DateTimeProvider::StartTime()
{
    const boost::posix_time::ptime& startTime = _StartTime();
    return boost::posix_time::to_iso_extended_string(startTime);
}

Time DateTimeProvider::ElapsedTime()
{
    const boost::posix_time::time_duration deltaT = _Now() - _StartTime();
    return deltaT.total_microseconds() * vsc::micro * vsc::seconds;
}

}
