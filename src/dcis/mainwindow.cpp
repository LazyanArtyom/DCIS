#include "mainwindow.h"

namespace dcis::ui {

// public
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    debugStream_ = std::make_unique<utils::DebugStream>(std::cout, txtConsole_);
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

    // File
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

    // Graph
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

    // Algorithms
    actionBFS_ = new QAction(this);
    actionBFS_->setObjectName("actBFS");
    actionDFS_ = new QAction(this);
    actionDFS_->setObjectName("actDFS");
    actAddNode_ = new QAction(this);

    // Central Widget
    centralTabWidget_ = new QTabWidget(this);
    setCentralWidget(centralTabWidget_);
    centralTabWidget_->setObjectName("centralTabWidget");
    centralTabWidget_->setCurrentIndex(1);
    /////////////////////////////////////////////////////////////////////

    // Entry widget
    entryWidget_ = new QWidget(centralTabWidget_);
    entryWidget_->setObjectName("entryWidget");
    centralTabWidget_->addTab(entryWidget_, tr("entryWidget"));

    QHBoxLayout *horizontalLayoutEntry_ = new QHBoxLayout(entryWidget_);
    entryWidget_->setLayout(horizontalLayoutEntry_);
    horizontalLayoutEntry_->setSpacing(6);
    horizontalLayoutEntry_->setContentsMargins(11, 11, 11, 11);

    // Connection widget
    QWidget* connectWidget = new QWidget(entryWidget_);
    horizontalLayoutEntry_->addWidget(connectWidget, Qt::AlignCenter);

    connectWidget->setMaximumWidth(500);

    QHBoxLayout *horizontalLayoutConnect = new QHBoxLayout(connectWidget);
    horizontalLayoutConnect->setSpacing(3);
    connectWidget->setLayout(horizontalLayoutConnect);

    ip_ = new QLineEdit(entryWidget_);
    horizontalLayoutConnect->addWidget(ip_);
    ip_->setObjectName("ip");
    ip_->setPlaceholderText(tr("server ip"));

    port_ = new QLineEdit(entryWidget_);
    horizontalLayoutConnect->addWidget(port_);
    port_->setObjectName("port");
    port_->setPlaceholderText(tr("port"));

    btnConnect_ = new QPushButton(tr("Connect"), entryWidget_);
    horizontalLayoutConnect->addWidget(btnConnect_);
    btnConnect_->setObjectName("btnConnect");
    /////////////////////////////////////////////////////////////////////

    // Working widget
    workingWidget_ = new QWidget(this);
    workingWidget_->setObjectName("workingWidget");
    centralTabWidget_->addTab(workingWidget_, tr("workingWidget"));

    QHBoxLayout* hLayoutWorking = new QHBoxLayout(workingWidget_);
    workingWidget_->setLayout(hLayoutWorking);

    txtConsole_ = new QTextEdit(workingWidget_);
    hLayoutWorking->addWidget(txtConsole_);

    txtConsole_->setObjectName("consoleText");
    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(txtConsole_->sizePolicy().hasHeightForWidth());
    txtConsole_->setSizePolicy(sizePolicyConsole);
    txtConsole_->setMinimumSize(QSize(0, 240));
    txtConsole_->setMaximumSize(QSize(16777215, 720));
    txtConsole_->setReadOnly(true);

    txtMessage_ = new QLineEdit(workingWidget_);
    hLayoutWorking->addWidget(txtMessage_);

    btnMsgSend_ = new QPushButton(tr("Send"), workingWidget_);
    hLayoutWorking->addWidget(btnMsgSend_);

    connect(btnMsgSend_, &QPushButton::clicked, this, &MainWindow::onBtnSendMsgClicked);


    /*
    menuFile_;
    menuGraph_;
    menuAdd_;
    menuEdit_;
    menuDelete_;
    menuAlgorithms_;

    menuBar_;*/
}

void MainWindow::onBtnSendMsgClicked()
{
    // send
    txtConsole_->insertPlainText(QString("\nSending message: ") + txtMessage_->text());
    client_->sendToServer(txtMessage_->text());
}

void MainWindow::onMsgReieved(const QString msg)
{
    txtConsole_->insertPlainText(msg);
}

// private

// slots

} // end namespace dcis::ui
