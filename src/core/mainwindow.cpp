#include "mainwindow.h"

#include <QVBoxLayout>

#include <iostream>
#include <string>
#include <vector>
namespace dcis::server {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("SERVER");
    //setWindowIcon(QIcon(":/Resources/icons/histogram.png"));
    setWindowTitle(tr("DCIS Server"));
    //connect(server_, &Server::sigPrintMsg, this, &MainWindow::onPrintMsg);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setHeightForWidth(hasHeightForWidth());
    setSizePolicy(sizePolicy);

    setMinimumSize(QSize(1024, 768));
    setMaximumSize(QSize(16777215, 16777215));
    showMaximized();

    // Central Widget
    centralTabWidget_ = new QTabWidget(this);
    setCentralWidget(centralTabWidget_);
    centralTabWidget_->setObjectName("centralTabWidget");
    /////////////////////////////////////////////////////////////////////

    // Entry widget
    QWidget *consoleWidget_ = new QWidget(centralTabWidget_);
    consoleWidget_->setObjectName("entryWidget");
    centralTabWidget_->addTab(consoleWidget_, tr("entryWidget"));

    QHBoxLayout *horizontalLayoutEntry = new QHBoxLayout(consoleWidget_);
    consoleWidget_->setLayout(horizontalLayoutEntry);
    horizontalLayoutEntry->setSpacing(6);
    horizontalLayoutEntry->setContentsMargins(11, 11, 11, 11);

    txtConsole_ = new QTextEdit(this);
    horizontalLayoutEntry->addWidget(txtConsole_);
    txtConsole_->setObjectName("consoleText");

    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(txtConsole_->sizePolicy().hasHeightForWidth());
    txtConsole_->setSizePolicy(sizePolicyConsole);

    txtConsole_->setMinimumSize(QSize(0, 240));
    txtConsole_->setMaximumSize(QSize(16777215, 720));
    txtConsole_->setReadOnly(true);

    debugStream_ = std::make_unique<utils::DebugStream>(std::cout, txtConsole_);
    server_ = new Server(this);
}

MainWindow::~MainWindow()
{
    server_->close();
    server_->deleteLater();
}

} // end namespace dcis::server
