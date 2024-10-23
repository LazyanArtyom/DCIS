#include "mainwindow.h"

// QT includes
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>

// STL includes
#include <iostream>


int main(int argc, char *argv[])
{
    bool headlessMode = false;

    for (int i = 1; i < argc; ++i) 
    {
        if (QString(argv[i]) == "--headless")
        {
            headlessMode = true;
            break;
        }
    }

    if (headlessMode)
    {
        std::cout << "Headless mode" << std::endl;
        QCoreApplication app(argc, argv);
        QCoreApplication::setApplicationName("DCIS Server");
        QCoreApplication::setApplicationVersion("0.0.1");

        // console
        dcis::common::utils::ILogger* terminalWidget_ = new dcis::common::utils::LoggerCore();

        // server
        dcis::server::Server *server_ = new dcis::server::Server(terminalWidget_);
        server_->run(2323);

        return app.exec();
    }
    else
    {
        QApplication app(argc, argv);
        dcis::server::MainWindow w;
        w.show();
        return app.exec();
    }
}
