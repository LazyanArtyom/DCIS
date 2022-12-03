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
    centralWidget_ = new QStackedWidget(this);
    setCentralWidget(centralWidget_);
    centralWidget_->setObjectName("centralTabWidget");
    /////////////////////////////////////////////////////////////////////

    // Entry widget
    entryWidget_ = new QWidget(centralWidget_);
    entryWidget_->setObjectName("entryWidget");
    centralWidget_->addWidget(entryWidget_);

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
    connect(btnConnect_, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);
    /////////////////////////////////////////////////////////////////////

    // Working widget
    workingWidget_ = new QWidget(this);
    workingWidget_->setObjectName("workingWidget");
    centralWidget_->addWidget(workingWidget_);
    //////
    centralWidget_->setCurrentWidget(workingWidget_);
///////
    QHBoxLayout* hLayoutWorking = new QHBoxLayout(workingWidget_);
    workingWidget_->setLayout(hLayoutWorking);
    hLayoutWorking->setSpacing(6);
    hLayoutWorking->setContentsMargins(0, 11, 11, 11);

    // tool pane widget
    QDockWidget* dockWidget = new QDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    dockWidget->setObjectName("dockWidget");
    dockWidget->setMinimumWidth(100);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

    QSplitter* vSplitter = new QSplitter(workingWidget_);
    hLayoutWorking->addWidget(vSplitter);
    vSplitter->setLineWidth(2);
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->setChildrenCollapsible(false);

    // graph view widget
    graphView_ = new gui::GraphView;
    vSplitter->addWidget(graphView_);

    connect(graphView_, &gui::GraphView::sigNodeEdited, this, [this](const std::string& nodeName) {
        bool ok;
        QRegExp re("[a-zA-Z0-9]{1,30}");
        auto newName = QInputDialog::getText(this, "Rename node", "Name: ", QLineEdit::Normal,
                                              QString::fromStdString(graph_->getNextNodeName()), &ok);
        if (ok)
        {
            if (!re.exactMatch(newName)) {
                QMessageBox::critical(this, "Error",
                                      tr("Node's name contains only alphabetical or numeric characters\n")
                                      + tr("Length of the name mustn't be greater than 30 or smaller than 1"));
                return;
            }
            if (graph_->hasNode(newName.toStdString()))
                QMessageBox::critical(this, "Error", "This name has been used by another node");
            else {
                graph_->setNodeName(nodeName, newName.toStdString());
                emit sigGraphChanged();
            }
        }
    });

    //graphView_->setScene();
    graphView_->show();

    // console widget
    txtConsole_ = new QTextEdit(workingWidget_);
    vSplitter->addWidget(txtConsole_);
    txtConsole_->setObjectName("consoleText");
    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(txtConsole_->sizePolicy().hasHeightForWidth());
    txtConsole_->setSizePolicy(sizePolicyConsole);
    txtConsole_->setMinimumSize(QSize(0, 240));
    txtConsole_->setMaximumSize(QSize(16777215, 720));
    txtConsole_->setReadOnly(true);

    /*
    menuFile_;
    menuGraph_;
    menuAdd_;
    menuEdit_;
    menuDelete_;
    menuAlgorithms_;

    menuBar_;*/
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

// private

// slots

} // end namespace dcis::ui
