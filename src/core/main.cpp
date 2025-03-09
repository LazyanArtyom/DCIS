/*
 * Project Name: DCIS - Drone Collective Intelligence System
 * Copyright (C) 2022 Artyom Lazyan, Agit Atashyan, Davit Hayrapetyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"

// QT includes
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    bool headlessMode = false;

    for (int i = 1; i < argc; ++i) 
    {
        if (QString(argv[i]) == "--headless")
        {
            headlessMode = true;
            break;
        }
    }

    if (headlessMode)
    {
        QCoreApplication app(argc, argv);
        QCoreApplication::setApplicationName("DCIS Server");
        QCoreApplication::setApplicationVersion("1.0.0");

        // console
        dcis::common::utils::ILogger* terminalWidget_ = new dcis::common::utils::LoggerCore();

        // server
        dcis::core::Server *server_ = new dcis::core::Server(terminalWidget_);
        server_->run(2323);

        return app.exec();
    }
    else
    {
        QApplication app(argc, argv);
        dcis::core::MainWindow w;
        w.show();
        return app.exec();
    }
}
