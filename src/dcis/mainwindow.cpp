#include "mainwindow.h"

// Qt includes
#include <QDockWidget>
#include <QRegularExpression>

namespace dcis::ui {

// public
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

  //  debugStream_ = std::make_unique<utils::DebugStream>(std::cout, txtConsole_);
    client_ = new client::Client(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setObjectName("DCIS");
    //setWindowIcon(QIcon(":/Resources/icons/histogram.png"));
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

    createEntryWidget();
    createWorkingWiget();
    createGraphInfoWidget();
    createMenu();
    createToolBar();

    centralWidget_->setCurrentWidget(workingWidget_);
}

void MainWindow::setWorkspaceEnabled(bool enable)
{
    if (enable)
    {
        centralWidget_->setCurrentWidget(workingWidget_);
    }
    else
    {
        centralWidget_->setCurrentWidget(entryWidget_);
    }
}

void MainWindow::onConnectBtnClicked()
{
    setWorkspaceEnabled(true);
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
    QToolBar* pToolBar = addToolBar("Main ToolBar");

    QAction* actUploadImg = new QAction(QIcon(QPixmap(":/resources/upload.svg")), tr("Upload Image"));
    pToolBar->addAction(actUploadImg);

    // conections
    connect(actUploadImg, &QAction::triggered, graphEditingTool_, &gui::GraphEditingTool::onUploadImage);
}

void MainWindow::createEntryWidget()
{
    // Entry widget
    entryWidget_ = new QWidget(centralWidget_);
    entryWidget_->setObjectName("entryWidget");

    QWidget* connectWidget = new QWidget(entryWidget_);
    connectWidget->setMaximumWidth(500);

    ip_ = new QLineEdit(entryWidget_);
    ip_->setObjectName("ip");
    ip_->setPlaceholderText(tr("server ip"));

    port_ = new QLineEdit(entryWidget_);
    port_->setObjectName("port");
    port_->setPlaceholderText(tr("port"));

    btnConnect_ = new QPushButton(tr("Connect"), entryWidget_);
    btnConnect_->setObjectName("btnConnect");

    // Connections
    connect(btnConnect_, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);

    // Layouts
    centralWidget_->addWidget(entryWidget_);

    QHBoxLayout* horizontalLayoutEntry_ = new QHBoxLayout(entryWidget_);
    entryWidget_->setLayout(horizontalLayoutEntry_);
    horizontalLayoutEntry_->setSpacing(6);
    horizontalLayoutEntry_->setContentsMargins(11, 11, 11, 11);

    horizontalLayoutEntry_->addWidget(connectWidget, Qt::AlignCenter);

    QHBoxLayout* horizontalLayoutConnect = new QHBoxLayout(connectWidget);
    horizontalLayoutConnect->setSpacing(3);
    connectWidget->setLayout(horizontalLayoutConnect);

    horizontalLayoutConnect->addWidget(ip_);
    horizontalLayoutConnect->addWidget(port_);
    horizontalLayoutConnect->addWidget(btnConnect_);
}

void MainWindow::createWorkingWiget()
{
    // Working widget
    workingWidget_ = new QWidget(this);
    workingWidget_->setObjectName("workingWidget");

    // graph editing tool
    graphEditingTool_ = new gui::GraphEditingTool();

    // console widget
    txtConsole_ = new QTextEdit(workingWidget_);
    txtConsole_->setObjectName("consoleText");
    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(txtConsole_->sizePolicy().hasHeightForWidth());
    txtConsole_->setSizePolicy(sizePolicyConsole);
    txtConsole_->setMinimumSize(QSize(0, 240));
    txtConsole_->setMaximumSize(QSize(16777215, 720));
    txtConsole_->setReadOnly(true);

    // layouts
    centralWidget_->addWidget(workingWidget_);

    QHBoxLayout* hLayoutWorking = new QHBoxLayout(workingWidget_);
    workingWidget_->setLayout(hLayoutWorking);
    hLayoutWorking->setSpacing(6);
    hLayoutWorking->setContentsMargins(0, 11, 11, 11);

    QSplitter* vSplitter = new QSplitter(workingWidget_);
    hLayoutWorking->addWidget(vSplitter);
    vSplitter->setLineWidth(2);
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->setChildrenCollapsible(false);

    vSplitter->addWidget(graphEditingTool_);
    vSplitter->addWidget(txtConsole_);
    vSplitter->setSizes(QList<int>() << 700 << 200);
}

void MainWindow::createGraphInfoWidget()
{
    QDockWidget* dockWidget = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    dockWidget->setObjectName("dockWidget");
    dockWidget->setMinimumWidth(200);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable
                            | QDockWidget::DockWidgetMovable
                            | QDockWidget::DockWidgetClosable);

    // graph info widget
    graphInfo_ = new gui::GraphInfo(dockWidget);
    graphInfo_->setMinimumSize(200, 200);

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

    // layouts
    QVBoxLayout* vLayoutDock = new QVBoxLayout();
    dockWidget->setLayout(vLayoutDock);
    vLayoutDock->addWidget(graphInfo_);
}

} // end namespace dcis::ui
