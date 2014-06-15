/*!
 * \file GuiController.cpp
 * \brief Implementation of GuiController class.
 * \author Konstantin Androsov (INFN Pisa, Siena University)
 * \date 2014-06-15 created
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

#include <QMetaType>
#include "GuiController.h"

Q_DECLARE_METATYPE(vsc::exception)

GuiController::GuiController() :
    controllerThread(std::bind(&vsc::Controller::operator(), &controller)),
    mainWindow(controller)
{
    using namespace std::placeholders;
    controller.AddOnConnectSuccessfulCallback(std::bind(&GuiController::_ConnectSuccessful, this));
    controller.AddOnConnectFailedCallback(std::bind(&GuiController::_ConnectFailed, this, _1));

    qRegisterMetaType<vsc::exception>();

    QObject::connect(this, SIGNAL(ConnectSuccessful()), &mainWindow, SLOT(onConnectSuccessful()));
    QObject::connect(this, SIGNAL(ConnectFailed(vsc::exception)), &mainWindow, SLOT(onConnectFailed(vsc::exception)));

    mainWindow.show();
}

GuiController::~GuiController()
{
    controller.SendCommand(vsc::Controller::Exit);
    controllerThread.join();
}

void GuiController::_ConnectSuccessful()
{
    emit ConnectSuccessful();
}

void GuiController::_ConnectFailed(const vsc::exception& e)
{
    emit ConnectFailed(e);
}
