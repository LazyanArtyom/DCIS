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
#ifndef DCIS_UI_MAINWINDOW_H_
#define DCIS_UI_MAINWINDOW_H_

// App headers
#include <net/client.h>
#include <gui/graphview.h>
#include <gui/mapwidget.h>
#include <utils/terminalwidget.h>
#include <gui/uavsimulation/simulationview.h>

// QT headers
#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QSlider>
#include <QAction>
#include <QWidget>
#include <QMenuBar>
#include <QVariant>
#include <QLineEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QApplication>
#include <QStackedWidget>
#include <QGeoCoordinate>

namespace dcis::gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupUi();
    void setWorkspaceEnabled(bool enable);

    void uploadImage(const QString imagePath);

  protected:
    void resizeEvent(QResizeEvent *event) override;

  public slots:
    void onUpload();
    void onCreateGrid();
    void onStartAttack();
    void onClearCycles();
    void onGenerateGraph();
    void onStartExploration();
    void onConnectBtnClicked();
    void onDroneSimulation();
    void onLiveControl();
    void onUpdateGraph(const QJsonDocument &json);
    void onSimulateGraph(const QJsonDocument &json);
    void onLiveUpdateGraph(const QJsonDocument &json);


    void onSaveGraph();
    void onLoadGraph();
    void onGraphChanged();

  private:
    void createMenu();
    void createFolders();
    void createToolBar();
    void createMapWidget();
    void createEntryWidget();
    void createWorkingWiget();
    void createGraphInfoWidget();
    void createDroneSimulationWidget();

  private:
    QToolBar *toolBar_ = nullptr;
    MapWidget *mapWidget_ = nullptr;
    QWidget *simulationWidget_ = nullptr;
    QWidget *workingWidget_ = nullptr;
    QDockWidget *dockWidget_ = nullptr;
    QStackedWidget *centralWidget_ = nullptr;
    QWidget *entryWidget_ = nullptr;
    common::utils::ILogger *terminalWidget_ = nullptr;

    QWidget* liveControlContainer_ = nullptr;
    GraphView* liveGraphView_ = nullptr;
    UAVSimulationView* liveSimulationView_ = nullptr;

    QLabel *backgroundLabel_;
    QLineEdit *username_;
    QLineEdit *password_;
    QLineEdit *ipLineEdit_;
    QLineEdit *portLineEdit_;
    QPushButton *connectButton_;

    client::Client *client_;
    GraphView *graphView_;
    UAVSimulationView *simulationView_;
};

} // namespace dcis::gui
#endif // DCIS_UI_MAINWINDOW_H_
