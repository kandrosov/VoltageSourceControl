/*!
 * \file Controller.cc
 * \brief Implementation of Controller class.
 * \author Konstantin Androsov (INFN Pisa, Siena University)
 * \date 2014-05-24 created
 *
 * Copyright 2014 Konstantin Androsov <konstantin.androsov@gmail.com>
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
#include "Controller.h"

namespace vsc {

Controller::CommandHandler Controller::GetCommandHandler(Command command)
{
    typedef std::map<Command, CommandHandler> CommandMap;
    static CommandMap commandMap;
    if(!commandMap.size()) {
        commandMap[Exit] = &Controller::doExit;
        commandMap[Connect] = &Controller::doConnect;
        commandMap[Disconnect] = &Controller::doDisconnect;
        commandMap[EnableVoltage] = &Controller::doEnableVoltage;
        commandMap[DisableVoltage] = &Controller::doDisableVoltage;
    }
    return commandMap.at(command);
}

void Controller::AddOnMeasurementCallback(const OnMeasurementCallback& callback)
{
    onMeasurement.push_back(callback);
}

void Controller::AddOnComplianceCallback(const OnMeasurementCallback& callback)
{
    onCompliance.push_back(callback);
}

void Controller::AddOnErrorCallback(const OnErrorCallback& callback)
{
    onError.push_back(callback);
}

void Controller::operator()()
{

}

void Controller::SendCommand(Command command)
{
    commandQueue.push(command);
}

void Controller::onVoltageSourceMeasurement(const IVoltageSource::Measurement& measurement)
{
    Call(onMeasurement, measurement);
}

void Controller::doExit()
{

}

void Controller::doConnect()
{

}

void Controller::doDisconnect()
{

}

void Controller::doEnableVoltage()
{

}

void Controller::doDisableVoltage()
{

}

} // vsc
