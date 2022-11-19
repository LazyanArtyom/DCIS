#ifndef DCIS_SERVER_SERVER_H_
#define DCIS_SERVER_SERVER_H_

// App includes
#include <net/resource.h>
#include <utils/debugstream.h>

// QT includes
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>


namespace dcis::server {
using namespace common;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    using DataType = QByteArray;
    using SocketListType = QMap<qintptr, QTcpSocket*>;

    Server(QObject* parent = nullptr);
    ~Server();

    void run();
    void incomingConnection(qintptr socketDescriptor) override;

    void publish(resource::Header header, resource::Body body);
    void handle(resource::Header header, resource::Body body);

    void handleUnknown();
    void handleString(const QString str);
    void handleJson(const QJsonDocument json);

    void updateSockets();

public slots:
    void onReadyRead();
    void onDisconected();

private:
    DataType       data_;
    resource::Header header_;
    SocketListType sockets_;
    qintptr currentSocket_;
    quint16 nextBlockSize_;
};

} // end namespace dcis::server
#endif // DCIS_SERVER_SERVER_H_
