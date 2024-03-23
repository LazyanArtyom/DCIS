#include "mainwindow.h"

// App include
#include <gui/coordinputdialog.h>

// Qt includes
#include <QDockWidget>
#include <QRegularExpression>

namespace dcis::gui {

// public
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    setMinimumSize(QSize(1024, 768));
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
    terminalWidget_ = new utils::TerminalWidget(this);

    createEntryWidget();
    createWorkingWiget();
    createGraphInfoWidget();
    createMenu();
    createToolBar();

    setWorkspaceEnabled(false);
}

void MainWindow::setWorkspaceEnabled(bool enable)
{
    if (enable)
    {
        toolBar_->show();
        //dockWidget_->show();
        centralWidget_->setCurrentWidget(workingWidget_);
    }
    else
    {
        toolBar_->hide();
        //dockWidget_->hide();
        centralWidget_->setCurrentWidget(entryWidget_);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{

    if (entryWidget_ != nullptr)
    {
       entryWidget_->setGeometry(0, 0, width(), height()); // Set the initial size of the window
       entryWidget_->resize(size());

       backgroundLabel_->resize(entryWidget_->size());
    }


    QMainWindow::resizeEvent(event);
}

void MainWindow::onUpload()
{
    QString filePath = QFileDialog::getOpenFileName(
          this, ("Select an attachment"),
          QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
          ("File (*.json *.txt *.png *.jpg *.jpeg *.mp4)"));

    if (filePath.isEmpty())
    {
        terminalWidget_->appendText("You haven't selected any attachment! \n");
        return;
    }

    QImageReader imgReader(filePath);
    if (imgReader.canRead())
    {
        graph::Graph* graph = new graph::Graph(true);
        graphView_->updateGraph(graph);

        QImage img = imgReader.read();
        //imageEditor_->showImage(img);
        graphView_->setImage(img);
        CCoordInputDialog oDialog;
        if (oDialog.exec() == QDialog::Accepted)
        {
            leftTop_ = oDialog.leftTopCoordinate();
            rightBottom_ = oDialog.rightBottomCoordinate();
        }

        terminalWidget_->appendText("LeftTop: " + leftTop_ + " \n");
        terminalWidget_->appendText("RightBottom: " + rightBottom_ + " \n");
    }

    if (client_->checkServerConnected())
    {
        client_->sendAttachment(filePath, common::resource::Command::ServerPublish);
    }
    else
    {
        terminalWidget_->appendText("Server does not respond. Please reconnect! \n");
    }
}

void MainWindow::onCreateGrid()
{
   QDialog* popup = new QDialog(this);
   QVBoxLayout* layout = new QVBoxLayout;

   QCheckBox* checkBox1 = new QCheckBox("10x10");
   QCheckBox* checkBox2 = new QCheckBox("20x20");
   QCheckBox* checkBox3 = new QCheckBox("30x30");
   layout->addWidget(checkBox1);
   layout->addWidget(checkBox2);
   layout->addWidget(checkBox3);

   QLabel* rowsLabel = new QLabel("Rows:");
   QLineEdit* rowsLineEdit = new QLineEdit;
   QLabel* colsLabel = new QLabel("Cols:");
   QLineEdit* colsLineEdit = new QLineEdit;

   layout->addWidget(rowsLabel);
   layout->addWidget(rowsLineEdit);
   layout->addWidget(colsLabel);
   layout->addWidget(colsLineEdit);

   QPushButton* okButton = new QPushButton("OK");
   QPushButton* cancelButton = new QPushButton("Cancel");

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
        client_->sendCommand(common::resource::Command::ClientClearCycles);
    }

    terminalWidget_->appendText("Cycles Cleared\n");
}

void MainWindow::onGenerateGraph()
{
    terminalWidget_->appendText("Generating Graph\n");

    if (client_->checkServerConnected())
    {
        client_->sendCommand(common::resource::Command::ClientGenerateGraph);
    }

    terminalWidget_->appendText("Graph Generated\n");
}

void MainWindow::onStartExploration()
{
    terminalWidget_->appendText("Exploration Started\n");

    if (client_->checkServerConnected())
    {
        client_->sendCommand(common::resource::Command::ClientStartExploration);
    }
}
void MainWindow::onStartAttack()
{
    terminalWidget_->appendText("Attack Started\n");

    if (client_->checkServerConnected())
    {
        client_->sendCommand(common::resource::Command::ClientStartAttack);
    }
}

void MainWindow::onConnectBtnClicked()
{
    if (ipLineEdit_->text().isEmpty() || portLineEdit_->text().isEmpty()
       || username_->text().isEmpty() || password_->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Connection issue"), tr("Please fill all the fields."));
        return;
    }

    if (client_->connectToServer(ipLineEdit_->text(), portLineEdit_->text()))
    {
        client_->setUserName(username_->text());
        client_->setPassword(password_->text());
    }
    else
    {
        QMessageBox::warning(this, tr("Connection issue"), tr("Can't connect to the server."));
    }


    //setWorkspaceEnabled(true);
}

void MainWindow::onGraphChanged()
{
    graph::Graph* graph = graphView_->getGraph();
    graph->setLeftTop(leftTop_);
    graph->setRightBottom(rightBottom_);

    QJsonDocument jsonData = graph::Graph::toJSON(graph);
    client_->sendJson(jsonData, resource::Command::ServerPublish);
}

void MainWindow::onUpdateGraph(const QJsonDocument& json)
{
    terminalWidget_->appendText("*************** Updating graph ***************\n");
    graph::Graph* graph = graph::Graph::fromJSON(json);
    graphView_->updateGraph(graph);
}

void MainWindow::onSaveGraph()
{
    QString filePath;
    if (currentFilePath_.isEmpty())
    {
        QString defaultPath = QDir::currentPath() + "/untitled.json";
        filePath = QFileDialog::getSaveFileName(this, tr("Save File"), defaultPath, tr("JSON Files (*.json)"));
        if (filePath.isEmpty())
        {
            return;
        }
    } else
    {
        filePath = currentFilePath_;
    }

    graph::Graph* currentGraph = graphView_->getGraph();
    if (currentGraph == nullptr)
    {
        return;
    }

    if (graph::Graph::save(currentGraph, filePath))
    {
        currentFilePath_ = filePath; // Update the current file path
    }
}

void MainWindow::onLoadGraph()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("JSON Files (*.json)"));
    if (!filePath.isEmpty())
    {
        graph::Graph* graph = graph::Graph::load(filePath);
        graphView_->updateGraph(graph);
        currentFilePath_ = filePath; // Update the current file path
    }
    else
    {
        return;
    }

    onGraphChanged();
}

void MainWindow::createMenu()
{
    // file
    actSave_ = new QAction(this);
    actSave_->setObjectName("actSave");
    actExit_ = new QAction(this);
    actExit_->setObjectName("actExit");
    actSaveAs_ = new QAction(this);
    actSaveAs_->setObjectName("actSaveAs");
    actCredits_ = new QAction(this);
    actCredits_->setObjectName("actCredits");
    actNewGraph_ = new QAction(this);
    actNewGraph_->setObjectName("actNewGraph");
    actOpenGraph_ = new QAction(this);
    actOpenGraph_->setObjectName("actOpenGraph");

    // graph
    actAddNode_ = new QAction(this);
    actAddNode_->setObjectName("actAddNode");
    actAddEdge_ = new QAction(this);
    actAddEdge_->setObjectName("actAddEdge");
    actDelNode_ = new QAction(this);
    actDelNode_->setObjectName("actDelNode");
    actDelEdge_ = new QAction(this);
    actDelEdge_->setObjectName("actDelEdge");
    actEditEdge_ = new QAction(this);
    actEditEdge_->setObjectName("actEditEdge");

    // algorithms
    actionBFS_ = new QAction(this);
    actionBFS_->setObjectName("actBFS");
    actionDFS_ = new QAction(this);
    actionDFS_->setObjectName("actDFS");
    actAddNode_ = new QAction(this);

    /*
    menuFile_;
    menuGraph_;
    menuAdd_;
    menuEdit_;
    menuDelete_;
    menuAlgorithms_;

    menuBar_;*/
}

void MainWindow::createToolBar()
{
    // tool bar
    toolBar_ = addToolBar("Main ToolBar");
    toolBar_->setIconSize(QSize(32, 32));

    QAction* actUpload = new QAction(QIcon(QPixmap(":/resources/upload.png")), tr("Upload"));
    toolBar_->addAction(actUpload);

    QAction* actClearCycles = new QAction(QIcon(QPixmap(":/resources/clear_cycles.png")), tr("Clear Cycles"));
    toolBar_->addAction(actClearCycles);

    QAction* actGenerateGraph = new QAction(QIcon(QPixmap(":/resources/graph.png")), tr("Generate Drone Configs"));
    toolBar_->addAction(actGenerateGraph);

    QAction* actStartExploration = new QAction(QIcon(QPixmap(":/resources/exploration.png")), tr("StartExploration"));
    toolBar_->addAction(actStartExploration);

    QAction* actStartAttack = new QAction(QIcon(QPixmap(":/resources/attack.png")), tr("StartAttack"));
    toolBar_->addAction(actStartAttack);

    QAction* actSaveGraph = new QAction(QIcon(QPixmap(":/resources/save_graph.png")), tr("SaveGraph"));
    toolBar_->addAction(actSaveGraph);

    QAction* actLoadGraph = new QAction(QIcon(QPixmap(":/resources/load_graph.png")), tr("LoadGraph"));
    toolBar_->addAction(actLoadGraph);

    QAction* actCreateGrid = new QAction(QIcon(QPixmap(":/resources/grid.png")), tr("Grid"));
    toolBar_->addAction(actCreateGrid);

    // conections
    connect(actUpload, &QAction::triggered, this, &MainWindow::onUpload);
    connect(actClearCycles, &QAction::triggered, this, &MainWindow::onClearCycles);
    connect(actGenerateGraph, &QAction::triggered, this, &MainWindow::onGenerateGraph);
    connect(actStartExploration, &QAction::triggered, this, &MainWindow::onStartExploration);
    connect(actStartAttack, &QAction::triggered, this, &MainWindow::onStartAttack);
    connect(actLoadGraph, &QAction::triggered, this, &MainWindow::onLoadGraph);
    connect(actSaveGraph, &QAction::triggered, this, &MainWindow::onSaveGraph);
    connect(actCreateGrid, &QAction::triggered, this, &MainWindow::onCreateGrid);
}

void MainWindow::createEntryWidget()
{
    client_ = new client::Client(terminalWidget_, this);

    entryWidget_ = new QWidget(centralWidget_);
    entryWidget_->setObjectName("entryWidget");
    entryWidget_->setWindowTitle("Background Image Example");
    entryWidget_->setGeometry(0, 0, 500, 500);

    backgroundLabel_ = new QLabel(entryWidget_);
    backgroundLabel_->setPixmap(QPixmap(":/resources/background.png"));
    backgroundLabel_->setScaledContents(true);

    QLabel* textLabel = new QLabel("Drone Collective \n Intelligence System", entryWidget_);
    textLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    textLabel->setMaximumHeight(300);
    QFont montserratFont("Montserrat", 59, QFont::DemiBold);
    textLabel->setFont(montserratFont);

    username_ = new QLineEdit(entryWidget_);
    username_->setMaximumWidth(500);
    username_->setMinimumHeight(50);
    username_->setStyleSheet("font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    username_->setPlaceholderText("username");
    username_->setText("root");

    password_ = new QLineEdit(entryWidget_);
    password_->setMaximumWidth(500);
    password_->setMinimumHeight(50);
    password_->setStyleSheet("font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    password_->setPlaceholderText("password");
    password_->setText("root");

    ipLineEdit_ = new QLineEdit(entryWidget_);
    ipLineEdit_->setMaximumWidth(500);
    ipLineEdit_->setMinimumHeight(50);
    ipLineEdit_->setStyleSheet("font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    ipLineEdit_->setPlaceholderText("ip");
    ipLineEdit_->setText("127.0.0.1");

    portLineEdit_ = new QLineEdit(entryWidget_);
    portLineEdit_->setMaximumWidth(500);
    portLineEdit_->setMinimumHeight(50);
    portLineEdit_->setStyleSheet("font-size: 25px; padding: 10px; border-radius: 4px; color: #dadada; border: 1px solid #eee;");
    portLineEdit_->setPlaceholderText("port");
    portLineEdit_->setText("2323");

    connectButton_ = new QPushButton("Connect", entryWidget_);
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
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);

    // Layouts
    QVBoxLayout* mainLayout = new QVBoxLayout(entryWidget_);
    mainLayout->addWidget(textLabel);

    QVBoxLayout* inputLayout = new QVBoxLayout;
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

    connect(client_, &client::Client::sigShowImage, this, [this](const QImage& img) {

        terminalWidget_->appendText("*************** Showing image ***************\n");
        graphView_->setImage(img);
    });

    connect(client_, &client::Client::sigUserAccepted, this, [this](bool isAccepted) {

        if (isAccepted)
        {
            setWorkspaceEnabled(true);
        }
        else
        {
            QMessageBox::warning(this, tr("Connection issue"), tr("Incorrect username or password."));
        }
    });

    // Layouts
    centralWidget_->addWidget(workingWidget_);

    QHBoxLayout* hLayoutWorking = new QHBoxLayout(workingWidget_);
    workingWidget_->setLayout(hLayoutWorking);
    hLayoutWorking->setSpacing(6);
    hLayoutWorking->setContentsMargins(0, 11, 11, 11);

    QSplitter* vSplitter = new QSplitter(workingWidget_);
    hLayoutWorking->addWidget(vSplitter);
    vSplitter->setLineWidth(2);
    vSplitter->setOrientation(Qt::Vertical);
    //vSplitter->setChildrenCollapsible(false);

    vSplitter->addWidget(graphView_);
    hLayoutWorking->addWidget(terminalWidget_);
    vSplitter->addWidget(terminalWidget_);
    vSplitter->setSizes(QList<int>() << 700 << 200);
}

void MainWindow::createGraphInfoWidget()
{
    // graph info widget
    //graphInfo_ = new GraphInfo(dockWidget_);
    //graphInfo_->setMinimumSize(200, 200);

    // properties table
    //elementPropertiesTable_ = new gui::ElementPropertiesTable(graph_);

    // connections
    /*
    connect(graphScene_, &gui::GraphScene::sigGraphChanged, elementPropertiesTable_, &gui::ElementPropertiesTable::onGraphChanged);
    connect(graphView_, &gui::GraphView::sigUnSelected, elementPropertiesTable_, &gui::ElementPropertiesTable::onUnSelected);
    connect(this, &MainWindow::sigGraphChanged, elementPropertiesTable_, &gui::ElementPropertiesTable::onGraphChanged);
    connect(graphView_, &gui::GraphView::sigNodeSelected, elementPropertiesTable_, &gui::ElementPropertiesTable::onNodeSelected);
    connect(graphView_, &gui::GraphView::sigEdgeSelected, elementPropertiesTable_, &gui::ElementPropertiesTable::onEdgeSelected);
    */
}

} // end namespace dcis::ui
