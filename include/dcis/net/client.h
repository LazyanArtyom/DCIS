#ifndef DCIS_CLIENT_MAINWINDOW_H_
#define DCIS_CLIENT_MAINWINDOW_H_


// App includes
#include <net/resource.h>

// Qt includes
#include <QTcpSocket>


namespace dcis::client {
using namespace common;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    ~Client();

    void sendToServer(const QString msg);

public slots:
    void onReadyRead();

private:
    quint16 nextBlockSize_;
    QByteArray data_;
    QTcpSocket* socket_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_MAINWINDOW_H_
