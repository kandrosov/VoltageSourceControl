/*!
 * \file main.cpp
 * \brief Main entrance for VoltageSourceControl program.
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

#include <QApplication>
#include "exception.h"
#include "ConfigParameters.h"
#include "GuiController.h"

const std::string LOG_HEAD = "main";
const std::string DEFAULT_LOG_FILE_NAME = "info.log";
const std::string DEFAULT_ERROR_LOG_FILE_NAME = "error.log";
const std::string DEFAULT_DEBUG_LOG_FILE_NAME = "debug.log";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GuiController guiController;
    try {
        ConfigParameters& configParameters = ConfigParameters::ModifiableSingleton();
        configParameters.setDirectory(".");
        configParameters.setDebugFileName(DEFAULT_DEBUG_LOG_FILE_NAME);
        configParameters.setErrorFileName(DEFAULT_ERROR_LOG_FILE_NAME);
        configParameters.setLogFileName(DEFAULT_LOG_FILE_NAME);
        vsc::LogDebug().open( configParameters.FullDebugFileName() );
        vsc::LogError().open( configParameters.FullDebugFileName() );
        vsc::LogInfo ().open( configParameters.FullLogFileName() );

        vsc::LogInfo(LOG_HEAD) << "Starting... " << vsc::LogInfo::FullTimestampString() << std::endl;

        configParameters.ReadConfigParameterFile();
    } catch(vsc::exception& e) {
        guiController.getMainWindow().ReportError(e);
    }

    const int result = a.exec();
    vsc::LogInfo(LOG_HEAD) << "Exiting... " << vsc::LogInfo::FullTimestampString() << std::endl;
    return result;
}
