/*!
 * \file GuiController.h
 * \brief Definition of GuiController class.
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

#pragma once

#include <QObject>
#include <thread>
#include "MainWindow.h"

class GuiController : public QObject
{
    Q_OBJECT
public:
    explicit GuiController();
    virtual ~GuiController();
    MainWindow& getMainWindow() { return mainWindow; }
    
signals:
    void ConnectSuccessful();
    void ConnectFailed(const vsc::exception& e);
    void DisconnectSuccessful();
    void DisconnectFailed(const vsc::exception& e);

private:
    void _ConnectSuccessful();
    void _ConnectFailed(const vsc::exception& e);
    void _DisconnectSuccessful();
    void _DisconnectFailed(const vsc::exception& e);

private:
    vsc::Controller controller;
    std::thread controllerThread;
    MainWindow mainWindow;
};
