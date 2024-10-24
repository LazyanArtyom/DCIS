#include "mainwindow.h"

// APP includes
#include <graph/graph.h>

// QT includes
#include <QVBoxLayout>

// STL includes

namespace dcis::server
{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setObjectName("CoreUI");
    // setWindowIcon(QIcon(":/Resources/icons/histogram.png"));
    setWindowTitle(tr("DCIS Core"));

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

    // Console
    terminalWidget_ = new common::utils::LoggerWidget(this);

    // Layouts
    QWidget *consoleWidget_ = new QWidget(centralTabWidget_);
    consoleWidget_->setObjectName("Console");
    centralTabWidget_->addTab(consoleWidget_, tr("Console"));

    QHBoxLayout *horizontalLayoutEntry = new QHBoxLayout(consoleWidget_);
    consoleWidget_->setLayout(horizontalLayoutEntry);
    horizontalLayoutEntry->setSpacing(6);
    horizontalLayoutEntry->setContentsMargins(11, 11, 11, 11);

    horizontalLayoutEntry->addWidget(static_cast<common::utils::LoggerWidget*>(terminalWidget_));

    // server
    server_ = new Server(terminalWidget_, this);
    server_->run(2323);
}

MainWindow::~MainWindow()
{
    server_->close();
    server_->deleteLater();
}

} // end namespace dcis::server
