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

    void handle(resource::Header header, resource::Body body);

    void handleUnknown();
    void handleString(const QString str);
    void handleJson(const QJsonDocument json);

    void connectToServer();
    void sendToServer(resource::Header header, resource::Body body);

public slots:
    void onReadyRead();
    void onDisconected();

private:
    QByteArray data_;
    QTcpSocket* socket_;
    quint16 nextBlockSize_;
    resource::Header header_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_MAINWINDOW_H_
