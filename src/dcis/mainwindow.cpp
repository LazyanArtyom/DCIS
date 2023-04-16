#include "mainwindow.h"

// App include

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

    setWorkspaceEnabled(false);
    onConnectBtnClicked();
}

void MainWindow::setWorkspaceEnabled(bool enable)
{
    if (enable)
    {
        toolBar_->show();
        dockWidget_->show();
        centralWidget_->setCurrentWidget(workingWidget_);
    }
    else
    {
        toolBar_->hide();
        dockWidget_->hide();
        centralWidget_->setCurrentWidget(entryWidget_);
    }
}

Console* MainWindow::getConsole() const
{
    return console_;
}

void MainWindow::onUpload()
{
    QString filePath = QFileDialog::getOpenFileName(
          this, ("Select an attachment"),
          QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
          ("File (*.json *.txt *.png *.jpg *.jpeg *.mp4)"));

    if (filePath.isEmpty())
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Warning, "You haven't selected any attachment!");
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        resource::Header header;
        header.resourceType = resource::ResourceType::Attachment;
        header.command = resource::Command::Publish;
        header.fileName = fileName;

        QImageReader imgReader(filePath);
        if (imgReader.canRead())
        {
            QImage img = imgReader.read();
            graphEditingTool_->showImage(img);
        }

        QByteArray headerData;
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        ds << header;

        // header size 128 bytes
        headerData.resize(resource::Header::HEADER_SIZE);

        QByteArray data = file.readAll();
        header.bodySize = data.size();

        data.prepend(headerData);

        client_->sendToServer(data);
    }
    else
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Warning, "Can't open file!");
        return;
    }
}

void MainWindow::onConnectBtnClicked()
{
    ip_->setText("127.0.0.1");
    port_->setText("2323");

    if (client_->connectToServer(ip_->text(), port_->text()))
    {
        setWorkspaceEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, tr("Connection issue"), tr("Can't connect to the server."));
    }
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

    QAction* actUpload = new QAction(QIcon(QPixmap(":/resources/upload.svg")), tr("Upload"));
    toolBar_->addAction(actUpload);

    // conections
    connect(actUpload, &QAction::triggered, this, &MainWindow::onUpload);
}

void MainWindow::createEntryWidget()
{
    // client
    client_ = new client::Client(this);

    // entry widget
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

    // connections
    connect(btnConnect_, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);

    // layouts
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
    // working widget
    workingWidget_ = new QWidget(this);
    workingWidget_->setObjectName("workingWidget");

    // graph editing tool
    graphEditingTool_ = new GraphEditingTool();

    // console
    console_ = new Console();
    utils::DebugStream::getInstance().setEditor(console_);

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
    vSplitter->addWidget(console_);
    vSplitter->setSizes(QList<int>() << 700 << 200);
}

void MainWindow::createGraphInfoWidget()
{
    dockWidget_ = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget_);
    dockWidget_->setObjectName("dockWidget");
    dockWidget_->setMinimumWidth(200);
    dockWidget_->setFeatures(QDockWidget::DockWidgetFloatable
                            | QDockWidget::DockWidgetMovable
                            | QDockWidget::DockWidgetClosable);

    // graph info widget
    graphInfo_ = new GraphInfo(dockWidget_);
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
}

} // end namespace dcis::ui
