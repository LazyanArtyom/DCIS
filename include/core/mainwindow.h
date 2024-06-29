#ifndef DCIS_SERVER_MAINWINDOW_H_
#define DCIS_SERVER_MAINWINDOW_H_

// App includes
#include <net/server.h>
#include <utils/terminalwidget.h>

// STL includes
#include <memory>

// Qt includes
#include <QMainWindow>
#include <QTabWidget>

namespace dcis::server
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private:
    Server *server_;
    QTabWidget *centralTabWidget_;
    common::utils::TerminalWidget *terminalWidget_;

    // std::unique_ptr<utils::DebugStream> debugStream_;
};

} // end namespace dcis::server
#endif // DCIS_SERVER_MAINWINDOW_H_
