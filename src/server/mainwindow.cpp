#include "mainwindow.h"

#include <test1.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    A aa;
    aa.foo();
}

MainWindow::~MainWindow()
{
}

