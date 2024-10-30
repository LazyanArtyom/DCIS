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
#include <gui/graphinfo.h>
#include <gui/graphview.h>
#include <utils/terminalwidget.h>
#include <gui/elementpropertiestable.h>

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
    void onUpdateGraph(const QJsonDocument &json);

    void onSaveGraph();
    void onLoadGraph();
    void onGraphChanged();

  private:
    void createMenu();
    void createToolBar();
    void createEntryWidget();
    void createWorkingWiget();
    void createGraphInfoWidget();

  private:
    QAction *actSave_;
    QAction *actExit_;
    QAction *actSaveAs_;
    QAction *actionBFS_;
    QAction *actionDFS_;
    QAction *actAddNode_;
    QAction *actAddEdge_;
    QAction *actDelNode_;
    QAction *actDelEdge_;
    QAction *actCredits_;
    QAction *actNewGraph_;
    QAction *actEditEdge_;
    QAction *actOpenGraph_;

    QMenu *menuFile_;
    QMenu *menuGraph_;
    QMenu *menuAdd_;
    QMenu *menuEdit_;
    QMenu *menuDelete_;
    QMenu *menuAlgorithms_;

    QMenuBar *menuBar_;
    QToolBar *toolBar_;

    // Workspace
    QWidget *workingWidget_;
    QDockWidget *dockWidget_;
    QStackedWidget *centralWidget_;
    QWidget *entryWidget_ = nullptr;
    common::utils::ILogger *terminalWidget_;

    // Entry Widget
    QLabel *backgroundLabel_;
    QLineEdit *username_;
    QLineEdit *password_;
    QLineEdit *ipLineEdit_;
    QLineEdit *portLineEdit_;
    QPushButton *connectButton_;

    client::Client *client_;
    GraphInfo *graphInfo_;
    GraphView *graphView_;

    QString currentFilePath_ = "";

    QString leftTop_;
    QString rightBottom_;
    ElementPropertiesTable *elementPropertiesTable_;
};

} // namespace dcis::gui
#endif // DCIS_UI_MAINWINDOW_H_
