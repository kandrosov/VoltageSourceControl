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

#include "log.h"
#include "ConfigParameters.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

const std::string LOG_HEAD = "main";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QObject::connect(this, SIGNAL(ConnectSuccessful()), this, SLOT(on_connectSuccessful()));
    QObject::connect(this, SIGNAL(ConnectFailed(vsc::exception)), this, SLOT(on_connectFailed(vsc::exception)));

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
    ui->statusBar->showMessage(QString::fromStdString(error.full_message()));
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
    try {
        voltageSource = vsc::VoltageSourceFactory::Get();
    } catch(vsc::exception& e) {
        ReportError(e);
        return;
    }
    ui->pushButtonEnableVoltage->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectSuccessful()
{
    ui->pushButtonEnableVoltage->setEnabled(true);
}

void MainWindow::on_connectFailed(const vsc::exception& e)
{
    ReportError(e);
}


void MainWindow::on_pushButtonEnableVoltage_clicked()
{

}

void MainWindow::on_pushButtonConnect_clicked()
{
    UpdateVoltageSource();
}
