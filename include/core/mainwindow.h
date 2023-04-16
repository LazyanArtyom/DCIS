#ifndef DCIS_SERVER_MAINWINDOW_H_
#define DCIS_SERVER_MAINWINDOW_H_

#include <QMainWindow>

// App includes
#include <net/server.h>

// STL includes
#include <memory>

// Qt includes
#include <QTextEdit>
#include <QTabWidget>


namespace dcis::server {
using namespace common;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private:
    Server*     server_;
    QTextEdit*  txtConsole_;
    QTabWidget* centralTabWidget_;

    //std::unique_ptr<utils::DebugStream> debugStream_;
};

} // end namespace dcis::server
#endif // DCIS_SERVER_MAINWINDOW_H_
