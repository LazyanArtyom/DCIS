#ifndef DCIS_SERVER_SERVER_H_
#define DCIS_SERVER_SERVER_H_

// App includes
#include <net/resource.h>
#include <resourcehandler.h>
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

    template<typename T>
    void publish(resource::Header header, resource::Body<T> body);

public slots:
    void onReadyRead();
    void onDisconected(qintptr socketDescriptor);

signals:
    void sigPrintMsg(const QString msg);

private:
    void sendToClient(const QString str);

private:
    DataType       data_;
    SocketListType sockets_;
    qintptr currentSocket_;
    quint16 nextBlockSize_;
    ResourceHandler resourceHandler_;
};

template<typename T>
void Server::publish(resource::Header header, resource::Body<T> body)
{
    data_.clear();
    QDataStream output(&data_, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_4);

    output << header << body;

    for (auto &socket : sockets_)
    {
        socket->write(data_);
    }
}

} // end namespace dcis::server
#endif // DCIS_SERVER_SERVER_H_
