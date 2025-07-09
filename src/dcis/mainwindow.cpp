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

// APP include
#include <net/clientsenders.h>
#include <config/configmanager.h>
#include <gui/coordinputdialog.h>

// QT includes
#include <QDockWidget>
#include <QRegularExpression>

namespace dcis::gui
{

// public
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setObjectName("DCIS");
    setWindowIcon(QIcon(":/resources/drone.png"));
    setWindowTitle(tr("DCIS - Drone Collective Intelligence System"));

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setHeightForWidth(hasHeightForWidth());
    setSizePolicy(sizePolicy);

    // setMinimumSize(QSize(1024, 768));
    setMaximumSize(QSize(16777215, 16777215));
    showMaximized();

    // Central Widget
    centralWidget_ = new QStackedWidget(this);
    setCentralWidget(centralWidget_);
    centralWidget_->setObjectName("centralTabWidget");

    /*dockWidget_ = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget_);
    dockWidget_->setObjectName("dockWidget");
    dockWidget_->setMinimumWidth(200);
    dockWidget_->setFeatures(QDockWidget::DockWidgetFloatable
                            | QDockWidget::DockWidgetMovable
                            | QDockWidget::DockWidgetClosable);
                            */

    // Console
    terminalWidget_ = new utils::LoggerWidget(this);

    createFolders();
    createMapWidget();
    createEntryWidget();
    createWorkingWiget();
    createGraphInfoWidget();
    createDroneSimulationWidget();
    createMenu();
    createToolBar();

    setWorkspaceEnabled(false);
}

void MainWindow::setWorkspaceEnabled(bool enable)
{
    if (enable)
    {
        if (username_->text() == "root" && password_->text() == "root")
        {
            toolBar_->show();
            centralWidget_->setCurrentWidget(mapWidget_);
        }
        else
        {
            // dockWidget_->show();
            centralWidget_->setCurrentWidget(workingWidget_);
        }
    }
    else
    {
        toolBar_->hide();
        // dockWidget_->hide();
        centralWidget_->setCurrentWidget(entryWidget_);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

    if (entryWidget_ != nullptr)
    {
        entryWidget_->setGeometry(0, 0, width(), height()); // Set the initial size of the window
        entryWidget_->resize(size());

        backgroundLabel_->resize(entryWidget_->size());
    }

    QMainWindow::resizeEvent(event);
}

void MainWindow::uploadImage(const QString imagePath)
{
    qDebug() << "***** OPENING IMAGE : " << imagePath;
    if (imagePath.isEmpty())
    {
        terminalWidget_->appendText("Image doesn't exist ! \n");
        return;
    }

    QImageReader imgReader(imagePath);
    if (imgReader.canRead())
    {
        graph::Graph *graph = new graph::Graph(false);
        graphView_->updateGraph(graph);

        QImage img = imgReader.read();
        graphView_->setImage(img);
    }
}

void MainWindow::onUpload()
{
    QString uploadedImagesPath = common::config::ConfigManager::getConfig("uploaded_images_path").toString();
    QString filePath = QFileDialog::getOpenFileName(this, 
                                                   tr("Select an attachment"),
                                                       uploadedImagesPath,
                                                    ("File (*.json *.txt *.png *.jpg *.jpeg *.mp4)"));
    centralWidget_->setCurrentWidget(workingWidget_);
    uploadImage(filePath);

    CoordInputDialog cordDialog;
    if (cordDialog.exec() == QDialog::Accepted)
    {
        mapWidget_->setLeftTop(cordDialog.leftTopCoordinate());
        mapWidget_->setRightBottom(cordDialog.rightBottomCoordinate());
    }

    if (client_->checkServerConnected())
    {
        client::AttachmentSender sender(client_->getSocket(), client_);
        sender.sendToServer(filePath, common::resource::command::server::Publish);
    }
    else
    {
        terminalWidget_->appendText("Server does not respond. Please reconnect! \n");
    }
}

void MainWindow::onCreateGrid()
{
    QDialog *popup = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout;

    QCheckBox *checkBox1 = new QCheckBox("10x10");
    QCheckBox *checkBox2 = new QCheckBox("20x20");
    QCheckBox *checkBox3 = new QCheckBox("30x30");
    layout->addWidget(checkBox1);
    layout->addWidget(checkBox2);
    layout->addWidget(checkBox3);

    QLabel *rowsLabel = new QLabel("Rows:");
    QLineEdit *rowsLineEdit = new QLineEdit;
    QLabel *colsLabel = new QLabel("Cols:");
    QLineEdit *colsLineEdit = new QLineEdit;

    layout->addWidget(rowsLabel);
    layout->addWidget(rowsLineEdit);
    layout->addWidget(colsLabel);
    layout->addWidget(colsLineEdit);

    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));

    connect(okButton, &QPushButton::clicked, popup, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, popup, &QDialog::reject);

    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    popup->setLayout(layout);
    int result = popup->exec();

    if (result == QDialog::Accepted)
    {
        if (checkBox1->isChecked())
        {
            graphView_->generateGraph(10, 10);
        }
        else if (checkBox2->isChecked())
        {
            graphView_->generateGraph(20, 20);
        }
        else if (checkBox3->isChecked())
        {
            graphView_->generateGraph(30, 30);
        }
        else
        {
            graphView_->generateGraph(rowsLineEdit->text().toInt(), colsLineEdit->text().toInt());
        }
    }
    else
    {
        // Handle the Cancel button click
    }
}

void MainWindow::onClearCycles()
{
    terminalWidget_->appendText("Map generated, Clearing Cycles\n");

    if (client_->checkServerConnected())
    {
        client::CommandSender sender(client_->getSocket(), client_);
        sender.sendToServer(common::resource::command::server::ClearCycles);
    }

    terminalWidget_->appendText("Cycles Cleared\n");
}

void MainWindow::onGenerateGraph()
{
    terminalWidget_->appendText("Generating Graph\n");

    if (client_->checkServerConnected())
    {
        client::CommandSender sender(client_->getSocket(), client_);
        sender.sendToServer(common::resource::command::server::GenerateGraph);
    }

    terminalWidget_->appendText("Graph Generated\n");
}

void MainWindow::onStartExploration()
{
    terminalWidget_->appendText("Exploration Started\n");

    if (client_->checkServerConnected())
    {
        client::CommandSender sender(client_->getSocket(), client_);
        sender.sendToServer(common::resource::command::server::StartExploration);
    }
}
void MainWindow::onStartAttack()
{
    terminalWidget_->appendText("Attack Started\n");

    if (client_->checkServerConnected())
    {
        client::CommandSender sender(client_->getSocket(), client_);
        sender.sendToServer(common::resource::command::server::StartAttack);
    }
}

void MainWindow::onConnectBtnClicked()
{
    if (ipLineEdit_->text().isEmpty() || portLineEdit_->text().isEmpty() || username_->text().isEmpty() ||
        password_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Connection issue"), tr("Please fill all the fields."));
        return;
    }

    client_->setUserName(username_->text());
    client_->setPassword(password_->text());

    if (!client_->connectToServer(ipLineEdit_->text(), portLineEdit_->text()))
    {
        QMessageBox::warning(this, tr("Connection issue"), tr("Can't connect to the server."));
    }
}

void MainWindow::onDroneSimulation()                    
{                                                       
    graph::Graph *graph = graphView_->getGraph();       
    centralWidget_->setCurrentWidget(simulationWidget_);
    simulationView_->setImage(graphView_->getImage());
    simulationView_->setGraph(graph);

    client::CommandSender sender(client_->getSocket(), client_);
    sender.sendToServer(resource::command::server::StartSimulation);
}

void MainWindow::onLiveControl()
{
    if (!liveControlContainer_)
    {
        // Create container widget
        liveControlContainer_ = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout(liveControlContainer_);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        // Create new views
        liveGraphView_ = new GraphView();
        liveSimulationView_ = new UAVSimulationView();

        // Copy current graph & image
        common::graph::Graph* currentGraph = graphView_->getGraph();
        QImage currentImage = graphView_->getImage();

        liveGraphView_->setImage(currentImage);
        liveGraphView_->updateGraph(currentGraph);
        
        liveSimulationView_->setImage(currentImage);
        liveSimulationView_->setGraph(currentGraph);

        // Splitter
        QSplitter *splitter = new QSplitter(Qt::Horizontal, liveControlContainer_);
        splitter->addWidget(liveGraphView_);
        splitter->addWidget(liveSimulationView_);
        splitter->setSizes(QList<int>() << width() / 2 << width() / 2);

        layout->addWidget(splitter);
        centralWidget_->addWidget(liveControlContainer_);
    }

    // Show Live Control layout
    centralWidget_->setCurrentWidget(liveControlContainer_);

    client::CommandSender sender(client_->getSocket(), client_);
    sender.sendToServer(resource::command::server::StartLiveControl);
}

void MainWindow::onGraphChanged()
{
    graph::Graph *graph = graphView_->getGraph();
    graph->setLeftTop(mapWidget_->getLeftTop());
    graph->setRightBottom(mapWidget_->getRightBottom());

    QJsonDocument jsonData = graph::Graph::toJSON(graph);
    client::JsonSender sender(client_->getSocket(), client_);
    sender.sendToServer(jsonData, resource::command::server::Publish);
}

void MainWindow::onUpdateGraph(const QJsonDocument &json)
{
    graph::Graph *graph = graph::Graph::fromJSON(json);
    graphView_->updateGraph(graph);
}

void MainWindow::onSimulateGraph(const QJsonDocument &json)
{
    graph::Graph *graph = graph::Graph::fromJSON(json);
    simulationView_->setGraph(graph);
}

void MainWindow::onLiveUpdateGraph(const QJsonDocument &json)
{
    graph::Graph *graph = graph::Graph::fromJSON(json);
    liveSimulationView_->setGraph(graph);
}

void MainWindow::onSaveGraph()
{
    QString savedGraphsPath = common::config::ConfigManager::getConfig("saved_graphs_path").toString();
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), savedGraphsPath, tr("JSON Files (*.json)"));
    if (!filePath.isEmpty())
    {
        graph::Graph *currentGraph = graphView_->getGraph();
        if (currentGraph == nullptr)
        {
            return;
        }

        graph::Graph::save(currentGraph, filePath);
    }
}

void MainWindow::onLoadGraph()
{
    centralWidget_->setCurrentWidget(workingWidget_);

    QString savedGraphsPath = common::config::ConfigManager::getConfig("saved_graphs_path").toString();
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), savedGraphsPath, tr("JSON Files (*.json)"));
    if (!filePath.isEmpty())
    {
        graph::Graph *graph = graph::Graph::load(filePath);
        graphView_->updateGraph(graph);
        onGraphChanged();
    }
}

void MainWindow::createMenu()
{
}

void MainWindow::createFolders()
{
    QString savedGraphsPath = common::config::ConfigManager::getConfig("saved_graphs_path").toString();
    QDir dir(savedGraphsPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QString uploadedImagesPath = common::config::ConfigManager::getConfig("uploaded_images_path").toString();
    dir.setPath(uploadedImagesPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
}

void MainWindow::createToolBar()
{
    // tool bar
    toolBar_ = addToolBar(tr("Main ToolBar"));
    toolBar_->setIconSize(QSize(32, 32));

    QAction *actUpload = new QAction(QIcon(QPixmap(":/resources/upload.png")), tr("Upload"));
    toolBar_->addAction(actUpload);

    QAction *actClearCycles = new QAction(QIcon(QPixmap(":/resources/clear_cycles.png")), tr("Clear Cycles"));
    toolBar_->addAction(actClearCycles);

    QAction *actGenerateGraph = new QAction(QIcon(QPixmap(":/resources/graph.png")), tr("Generate Drone Configs"));
    toolBar_->addAction(actGenerateGraph);

    QAction *actStartExploration = new QAction(QIcon(QPixmap(":/resources/exploration.png")), tr("StartExploration"));
    toolBar_->addAction(actStartExploration);

    QAction *actStartAttack = new QAction(QIcon(QPixmap(":/resources/attack.png")), tr("StartAttack"));
    toolBar_->addAction(actStartAttack);

    QAction *actSaveGraph = new QAction(QIcon(QPixmap(":/resources/save_graph.png")), tr("SaveGraph"));
    toolBar_->addAction(actSaveGraph);

    QAction *actLoadGraph = new QAction(QIcon(QPixmap(":/resources/load_graph.png")), tr("LoadGraph"));
    toolBar_->addAction(actLoadGraph);

    QAction *actCreateGrid = new QAction(QIcon(QPixmap(":/resources/grid.png")), tr("Grid"));
    toolBar_->addAction(actCreateGrid);

    QAction *actDroneSimulation = new QAction(QIcon(QPixmap(":/resources/drone_sim.png")), tr("DroneSimulation"));
    toolBar_->addAction(actDroneSimulation);                                                                 

    QAction *actLiveControl = new QAction(QIcon(QPixmap(":/resources/drone_cntrl.png")), tr("LiveControl"));
    toolBar_->addAction(actLiveControl);

    // conections
    connect(actUpload, &QAction::triggered, this, &MainWindow::onUpload);
    connect(actClearCycles, &QAction::triggered, this, &MainWindow::onClearCycles);
    connect(actGenerateGraph, &QAction::triggered, this, &MainWindow::onGenerateGraph);
    connect(actStartExploration, &QAction::triggered, this, &MainWindow::onStartExploration);
    connect(actStartAttack, &QAction::triggered, this, &MainWindow::onStartAttack);
    connect(actLoadGraph, &QAction::triggered, this, &MainWindow::onLoadGraph);
    connect(actSaveGraph, &QAction::triggered, this, &MainWindow::onSaveGraph);
    connect(actCreateGrid, &QAction::triggered, this, &MainWindow::onCreateGrid);
    connect(actDroneSimulation, &QAction::triggered, this, &MainWindow::onDroneSimulation);
    connect(actLiveControl, &QAction::triggered, this, &MainWindow::onLiveControl);
}

void MainWindow::createMapWidget()
{
    mapWidget_ = new MapWidget(centralWidget_);
    mapWidget_->setObjectName("mapWidget");
    mapWidget_->setWindowTitle(tr("Map"));

    centralWidget_->addWidget(mapWidget_);

    // Connections
    connect(mapWidget_, &MapWidget::sigDataReady, [this]() {
        QString imagePath = mapWidget_->getImagePath();
        centralWidget_->setCurrentWidget(workingWidget_);

        uploadImage(imagePath);
        if (client_->checkServerConnected())
        {
            client::AttachmentSender sender(client_->getSocket(), client_);
            sender.sendToServer(imagePath, common::resource::command::server::Publish);
        }
        else
        {
            terminalWidget_->appendText("Server does not respond. Please reconnect! \n");
        }
    });
}

void MainWindow::createEntryWidget()
{
    client_ = new client::Client(terminalWidget_, this);

    entryWidget_ = new QWidget(centralWidget_);
    entryWidget_->setObjectName("entryWidget");
    entryWidget_->setWindowTitle(tr("Background Image Example"));
    entryWidget_->setGeometry(0, 0, 500, 500);

    backgroundLabel_ = new QLabel(entryWidget_);
    backgroundLabel_->setPixmap(QPixmap(":/resources/background.png"));
    backgroundLabel_->setScaledContents(true);

    QLabel *textLabel = new QLabel(tr("Drone Collective \n Intelligence System"), entryWidget_);
    textLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    textLabel->setMaximumHeight(300);
    QFont montserratFont("Montserrat", 59, QFont::DemiBold);
    textLabel->setFont(montserratFont);

    username_ = new QLineEdit(entryWidget_);
    username_->setMaximumWidth(500);
    username_->setMinimumHeight(50);
    username_->setStyleSheet(
        "font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    username_->setPlaceholderText(tr("username"));
    username_->setText("root");

    password_ = new QLineEdit(entryWidget_);
    password_->setMaximumWidth(500);
    password_->setMinimumHeight(50);
    password_->setStyleSheet(
        "font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    password_->setPlaceholderText(tr("password"));
    password_->setText("root");
    password_->setEchoMode(QLineEdit::Password);

    ipLineEdit_ = new QLineEdit(entryWidget_);
    ipLineEdit_->setMaximumWidth(500);
    ipLineEdit_->setMinimumHeight(50);
    ipLineEdit_->setStyleSheet(
        "font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    ipLineEdit_->setPlaceholderText(tr("ip"));
    ipLineEdit_->setText("127.0.0.1");

    portLineEdit_ = new QLineEdit(entryWidget_);
    portLineEdit_->setMaximumWidth(500);
    portLineEdit_->setMinimumHeight(50);
    portLineEdit_->setStyleSheet(
        "font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    portLineEdit_->setPlaceholderText(tr("port"));
    portLineEdit_->setText("2323");

    connectButton_ = new QPushButton(tr("Connect"), entryWidget_);
    connectButton_->setStyleSheet("font-size: 20px; padding: 10px; background-color: #b8865e; color: #333;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #ad7e59;"
                                  "}");
    connectButton_->setCursor(Qt::PointingHandCursor); // Set hand cursor on hover
    connectButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connectButton_->setMaximumWidth(500);
    connectButton_->setMinimumHeight(30);

    entryWidget_->setStyleSheet("background: transparent;");
    backgroundLabel_->resize(entryWidget_->size());
    backgroundLabel_->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Connections
    connect(username_, &QLineEdit::returnPressed, connectButton_, &QPushButton::click);
    connect(password_, &QLineEdit::returnPressed, connectButton_, &QPushButton::click);
    connect(ipLineEdit_, &QLineEdit::returnPressed, connectButton_, &QPushButton::click);
    connect(portLineEdit_, &QLineEdit::returnPressed, connectButton_, &QPushButton::click);
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);

    connect(client_, &client::Client::sigUserAccepted, this, [this](bool isAccepted) {
        if (isAccepted)
        {
            setWorkspaceEnabled(true);
        }
        else
        {
            QMessageBox::warning(this, tr("Connection issue"),
                                 tr("Incorrect username or password or User already connected."));
        }
    });

    // Layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(entryWidget_);
    mainLayout->addWidget(textLabel);

    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputLayout->setSpacing(10);
    inputLayout->setAlignment(Qt::AlignCenter);

    inputLayout->addWidget(username_);
    inputLayout->addWidget(password_);
    inputLayout->addWidget(ipLineEdit_);
    inputLayout->addWidget(portLineEdit_);
    inputLayout->addWidget(connectButton_);

    mainLayout->addLayout(inputLayout);
    entryWidget_->setLayout(mainLayout);
    centralWidget_->addWidget(entryWidget_);
}

void MainWindow::createWorkingWiget()
{
    workingWidget_ = new QWidget(this);
    workingWidget_->setObjectName("workingWidget");

    // Graph view
    graphView_ = new GraphView();

    // Connections
    connect(graphView_, &GraphView::sigGraphChanged, this, &MainWindow::onGraphChanged);
    connect(graphView_, &GraphView::sigNodeMoved, this, &MainWindow::onGraphChanged);
    connect(client_, &client::Client::sigUpdateGraph, this, &MainWindow::onUpdateGraph);
    connect(client_, &client::Client::sigSimulateGraph, this, &MainWindow::onSimulateGraph);
    connect(client_, &client::Client::sigLiveUpdateGraph, this, &MainWindow::onLiveUpdateGraph);

    connect(client_, &client::Client::sigShowImage, this, [this](const QImage &img) {
        graphView_->setImage(img);
    });

    // Layouts
    centralWidget_->addWidget(workingWidget_);

    QHBoxLayout *hLayoutWorking = new QHBoxLayout(workingWidget_);
    workingWidget_->setLayout(hLayoutWorking);
    hLayoutWorking->setSpacing(6);
    hLayoutWorking->setContentsMargins(0, 11, 11, 11);

    QSplitter *vSplitter = new QSplitter(workingWidget_);
    hLayoutWorking->addWidget(vSplitter);
    vSplitter->setLineWidth(2);
    vSplitter->setOrientation(Qt::Vertical);
    // vSplitter->setChildrenCollapsible(false);

    vSplitter->addWidget(graphView_);
    hLayoutWorking->addWidget(static_cast<common::utils::LoggerWidget*>(terminalWidget_));
    vSplitter->addWidget(static_cast<common::utils::LoggerWidget*>(terminalWidget_));
    vSplitter->setSizes(QList<int>() << 700 << 200);
}

void MainWindow::createDroneSimulationWidget()   
{                                                
    simulationWidget_ = new QWidget(this);       
    simulationWidget_->setObjectName("widget");  
    QHBoxLayout* layout = new QHBoxLayout();     
    simulationWidget_->setLayout(layout);        
                                                 
    // Graph view                                
    simulationView_ = new UAVSimulationView();   
    layout->addWidget(simulationView_);          
                                                 
    // Layouts                                   
    centralWidget_->addWidget(simulationWidget_);
}

void MainWindow::createGraphInfoWidget()
{
    // graph info widget
    // graphInfo_ = new GraphInfo(dockWidget_);
    // graphInfo_->setMinimumSize(200, 200);

    // properties table
    // elementPropertiesTable_ = new gui::ElementPropertiesTable(graph_);

    // connections
    /*
    connect(graphScene_, &gui::GraphScene::sigGraphChanged, elementPropertiesTable_,
    &gui::ElementPropertiesTable::onGraphChanged); connect(graphView_, &gui::GraphView::sigUnSelected,
    elementPropertiesTable_, &gui::ElementPropertiesTable::onUnSelected); connect(this, &MainWindow::sigGraphChanged,
    elementPropertiesTable_, &gui::ElementPropertiesTable::onGraphChanged); connect(graphView_,
    &gui::GraphView::sigNodeSelected, elementPropertiesTable_, &gui::ElementPropertiesTable::onNodeSelected);
    connect(graphView_, &gui::GraphView::sigEdgeSelected, elementPropertiesTable_,
    &gui::ElementPropertiesTable::onEdgeSelected);
    */
}

} // namespace dcis::gui
