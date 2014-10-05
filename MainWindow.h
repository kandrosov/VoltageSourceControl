/*!
 * \file MainWindow.h
 * \brief Definition of MainWindow class.
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

#pragma once

#include <QMainWindow>
#include "exception.h"
#include "Controller.h"

namespace Ui {
class MainWindow;
}

enum class GuiControlStatus { Disconnected, Connecting, Disconnecting, Connected };

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(vsc::Controller& _controller);
    void ReportError(const vsc::exception& error);
    void ReportUpdate(const std::string& status_message, const std::string& detailed_message);
    void UpdateVoltageSource();
    ~MainWindow();

private:
    void SetControlStatus(GuiControlStatus s);

private:
    virtual void closeEvent(QCloseEvent *);

private slots:
    void on_pushButtonEnableVoltage_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

public slots:
    void onConnectSuccessful();
    void onConnectFailed(const vsc::exception& e);
    void onDisconnectSuccessful();
    void onDisconnectFailed(const vsc::exception& e);

private:
    Ui::MainWindow *ui;
    QPalette errorLabelPalette, normalLabelPalette;
    vsc::Controller *controller;
    GuiControlStatus currentControlStatus;
};
