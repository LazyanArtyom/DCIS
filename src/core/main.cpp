#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dcis::server::MainWindow w;
    w.show();
    return a.exec();
}
