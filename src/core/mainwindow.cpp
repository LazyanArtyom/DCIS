#include "mainwindow.h"

// App includes
#include <graph/graph.h>
#include <utils/debugstream.h>

// Qt includes
#include <QVBoxLayout>

// STL includes
#include <string>
#include <vector>
#include <iostream>

namespace dcis::server {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("CoreUI");
    //setWindowIcon(QIcon(":/Resources/icons/histogram.png"));
    setWindowTitle(tr("DCIS Core"));

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setHeightForWidth(hasHeightForWidth());
    setSizePolicy(sizePolicy);

    setMinimumSize(QSize(1024, 768));
    setMaximumSize(QSize(16777215, 16777215));
    showMaximized();

    // central Widget
    centralTabWidget_ = new QTabWidget(this);
    setCentralWidget(centralTabWidget_);
    centralTabWidget_->setObjectName("centralTabWidget");

    // console
    txtConsole_ = new QTextEdit(this);
    txtConsole_->setMinimumSize(QSize(0, 240));
    txtConsole_->setMaximumSize(QSize(16777215, 720));
    txtConsole_->setReadOnly(true);
    txtConsole_->setObjectName("consoleText");
    utils::DebugStream::getInstance().setEditor(txtConsole_);

    // connections

    // layouts
    QWidget* consoleWidget_ = new QWidget(centralTabWidget_);
    consoleWidget_->setObjectName("entryWidget");
    centralTabWidget_->addTab(consoleWidget_, tr("entryWidget"));

    QHBoxLayout* horizontalLayoutEntry = new QHBoxLayout(consoleWidget_);
    consoleWidget_->setLayout(horizontalLayoutEntry);
    horizontalLayoutEntry->setSpacing(6);
    horizontalLayoutEntry->setContentsMargins(11, 11, 11, 11);

    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(txtConsole_->sizePolicy().hasHeightForWidth());
    txtConsole_->setSizePolicy(sizePolicyConsole);

    horizontalLayoutEntry->addWidget(txtConsole_);

    // server
    server_ = new Server(this);
    server_->run(2323);
}

MainWindow::~MainWindow()
{
    server_->close();
    server_->deleteLater();
}

} // end namespace dcis::server
