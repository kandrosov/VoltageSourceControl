/*!
 * \file MainWindow.cpp
 * \brief Implementation of MainWindow class.
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

#include <functional>
#include "log.h"
#include "ConfigParameters.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

const std::string LOG_HEAD = "main";

MainWindow::MainWindow(vsc::Controller& _controller) :
    QMainWindow(nullptr), ui(new Ui::MainWindow), controller(&_controller)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabIV), false);

    normalLabelPalette = ui->labelStatus->palette();
    QBrush brush = normalLabelPalette.windowText();
    brush.setColor(Qt::green);
    normalLabelPalette.setBrush(QPalette::WindowText, brush);
    errorLabelPalette = normalLabelPalette;
    brush.setColor(Qt::red);
    errorLabelPalette.setBrush(QPalette::WindowText, brush);


    const auto& voltageSources = vsc::VoltageSourceFactory::GetNames();
    for(const auto& voltageSource : voltageSources)
        ui->comboBoxVoltageSource->addItem(QString::fromStdString(voltageSource));


}

void MainWindow::ReportError(const vsc::exception& error)
{
    vsc::LogError(error.header()) << "ERROR: " << error.message() << std::endl;
    ui->labelStatus->setText(QString::fromStdString(error.short_message()));
    ui->labelStatus->setPalette(errorLabelPalette);
    ui->statusBar->showMessage(QString::fromStdString(error.message()));
}

void MainWindow::ReportUpdate(const std::string& status_message, const std::string& detailed_message)
{
    vsc::LogError(LOG_HEAD) << detailed_message << std::endl;
    ui->labelStatus->setText(QString::fromStdString(status_message));
    ui->labelStatus->setPalette(normalLabelPalette);
    ui->statusBar->showMessage(QString::fromStdString(detailed_message));
}

void MainWindow::UpdateVoltageSource()
{
    ConfigParameters& configParameters = ConfigParameters::ModifiableSingleton();
    const std::string newName = ui->comboBoxVoltageSource->currentText().toStdString();
    configParameters.setVoltageSource(newName);
    ReportUpdate("Connecting...", "Connecting to the voltage source.");
    ui->pushButtonConnect->setEnabled(false);
    controller->SendCommand(vsc::Controller::Connect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectSuccessful()
{
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonEnableVoltage->setEnabled(true);
}

void MainWindow::onConnectFailed(const vsc::exception& e)
{
    ReportError(e);
    ui->pushButtonConnect->setEnabled(true);
}

void MainWindow::on_pushButtonEnableVoltage_clicked()
{

}

void MainWindow::on_pushButtonConnect_clicked()
{
    UpdateVoltageSource();
}
