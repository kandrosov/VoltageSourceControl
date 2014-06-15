/*!
 * \file Controller.h
 * \brief Definition of Controller class.
 * \author Konstantin Androsov (INFN Pisa, Siena University)
 * \date 2014-05-26 created
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

#pragma once

#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include "exception.h"
#include "VoltageSourceFactory.h"

namespace vsc {
class Controller {
public:
    enum Command { Exit, Connect, Disconnect, EnableVoltage, DisableVoltage };
    typedef std::function<void (const IVoltageSource::Measurement&)> OnMeasurementCallback;
    typedef std::function<void (const vsc::exception&)> OnErrorCallback;
    typedef std::function<void ()> OnEventCallback;
    typedef VoltageSourceFactory::Pointer VoltageSourcePtr;
    typedef void (Controller::* CommandHandler)();

private:
    typedef std::vector<OnMeasurementCallback> MeasurementCallbackVector;
    typedef std::vector<OnErrorCallback> ErrorCallbackVector;
    typedef std::vector<OnEventCallback> EventCallbackVector;

    static CommandHandler GetCommandHandler(Command command);

public:
    Controller();
    ~Controller();
    void AddOnMeasurementCallback(const OnMeasurementCallback& callback) { AddCallback(onMeasurement, callback); }
    void AddOnComplianceCallback(const OnMeasurementCallback& callback)  { AddCallback(onCompliance, callback); }
    void AddOnErrorCallback(const OnErrorCallback& callback) { AddCallback(onError, callback); }
    void AddOnConnectSuccessfulCallback(const OnEventCallback& callback) { AddCallback(onConnectSuccessful, callback); }
    void AddOnConnectFailedCallback(const OnErrorCallback& callback) { AddCallback(onConnectFailed, callback); }

    void operator()();
    void SendCommand(Command command);

private:
    void onVoltageSourceMeasurement(const IVoltageSource::Measurement& measurement);

    template<typename Callback>
    void AddCallback(std::vector<Callback>& callbacks, const Callback& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        callbacks.push_back(callback);
    }

    template<typename CallbackVector, typename ...Arguments>
    void Call(const CallbackVector& callbacks, Arguments... arguments)
    {
        for(const auto& callback : callbacks)
            callback(arguments...);
    }

    void doExit();
    void doConnect();
    void doDisconnect();
    void doEnableVoltage();
    void doDisableVoltage();


private:
    MeasurementCallbackVector onMeasurement, onCompliance;
    ErrorCallbackVector onError, onConnectFailed;
    EventCallbackVector onConnectSuccessful;
    std::recursive_mutex mutex;
    std::condition_variable_any controlStateChange;
    std::queue<Command> commandQueue;
    VoltageSourcePtr voltageSource;
    bool canRun, isRunning;
};

} // vsc
