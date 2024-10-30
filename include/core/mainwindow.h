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
#ifndef DCIS_SERVER_MAINWINDOW_H_
#define DCIS_SERVER_MAINWINDOW_H_

// App includes
#include <net/server.h>
#include <utils/terminalwidget.h>

// STL includes

// Qt includes
#include <QMainWindow>
#include <QTabWidget>

namespace dcis::server
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private:
    Server *server_;
    QTabWidget *centralTabWidget_;
    common::utils::ILogger *terminalWidget_;

    // std::unique_ptr<utils::DebugStream> debugStream_;
};

} // end namespace dcis::server
#endif // DCIS_SERVER_MAINWINDOW_H_
