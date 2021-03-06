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
#include "log.h"

namespace vsc {

Controller::CommandHandler Controller::GetCommandHandler(Command command)
{
    typedef std::map<Command, CommandHandler> CommandMap;
    static CommandMap commandMap;
    if(!commandMap.size()) {
        commandMap[Command::Exit] = &Controller::doExit;
        commandMap[Command::Connect] = &Controller::doConnect;
        commandMap[Command::Disconnect] = &Controller::doDisconnect;
        commandMap[Command::EnableVoltage] = &Controller::doEnableVoltage;
        commandMap[Command::DisableVoltage] = &Controller::doDisableVoltage;
    }
    return commandMap.at(command);
}

Controller::Controller()
    : canRun(true), isRunning(false)
{}

Controller::~Controller()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if(isRunning)
        THROW_VSC_EXCEPTION("Internal error", "Invalid usage. The object should not be destroyed while working thread is still running.");
    if(voltageSource)
        THROW_VSC_EXCEPTION("Internal error", "Invalid usage. The object should not be destroyed while there is an active connection to the voltage source.");

}

void Controller::operator()()
{
    std::unique_lock<std::recursive_mutex> lock(mutex);
    isRunning = true;
    while(canRun) {
        controlStateChange.wait(lock);
        while(commandQueue.size()) {
            const Command command = commandQueue.front();
            commandQueue.pop();
            const CommandHandler handler = GetCommandHandler(command);
            try {
                (this->*handler)();
            } catch(vsc::exception& e) {
                Call(onError, e);
            }
        }
    }

    isRunning = false;

}

void Controller::SendCommand(Command command)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        commandQueue.push(command);
    }
    controlStateChange.notify_one();
}

void Controller::onVoltageSourceMeasurement(const IVoltageSource::Measurement& measurement)
{
    Call(onMeasurement, measurement);
}

void Controller::doExit()
{
    canRun = false;
}

void Controller::doConnect()
{
    if(voltageSource) {
        const vsc::exception e("Controller", "Connection error", "Program is already connected to the voltage source");
        Call(onConnectFailed, e);
        return;
    }
    try {
        voltageSource = vsc::VoltageSourceFactory::Create();
        Call(onConnectSuccessful);
    } catch(vsc::exception& e) {
        Call(onConnectFailed, e);
    }
}

void Controller::doDisconnect()
{
    try {
        if(voltageSource)
            voltageSource->Off();
        Call(onDisconnectSuccessful);
    } catch(vsc::exception& e) {
        Call(onDisconnectFailed, e);
    }
    voltageSource = VoltageSourcePtr();
}

void Controller::doEnableVoltage()
{

}

void Controller::doDisableVoltage()
{

}

} // vsc
