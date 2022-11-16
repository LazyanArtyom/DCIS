#include <server.h>

#include <QJsonDocument>

#include <iostream>

#include <net/resource.h>


namespace dcis::server {

// public
Server::Server(QObject* parent)
    : QTcpServer(parent), resourceHandler_(this)
{
    nextBlockSize_ = 0;
    run();
}

Server::~Server()
{
    utils::log(utils::LogLevel::INFO, "End Server.");

    QMapIterator<qintptr, QTcpSocket *> socket(sockets_);
    while (socket.hasNext())
    {
        socket.value()->close();
        socket.value()->deleteLater();
    }
}

void Server::run()
{
    if (listen(QHostAddress::Any, 2323))
    {
        utils::log(utils::LogLevel::INFO, "Start Server, listening port: ");
    }
    else
    {
        utils::log(utils::LogLevel::ERROR, "Couldn't start server.");
    }
}

// slots
void Server::onReadyRead()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_4);

    if (input.status() == QDataStream::Ok)
    {
        resource::Header head;
        while (true)
        {
            if (nextBlockSize_ == 0)
            {
                if (socket->bytesAvailable() < (qint64)sizeof(resource::Header))
                {
                    break;
                }
                input >> head;
                nextBlockSize_ = head.bodySize;
            }

            if (socket->bytesAvailable() < nextBlockSize_)
            {
                break;
            }

            resource::Body<QJsonDocument> body;
            input >> body;

            nextBlockSize_ = 0;
            resourceHandler_.handle(head, body);
            break;
        }

    }
    else
    {
        utils::log(utils::LogLevel::ERROR, "Datastream error.");
    }
}

void Server::onDisconected(qintptr socketDescriptor)
{
    utils::log(utils::LogLevel::ERROR, "Client disonected.");

    sockets_[socketDescriptor]->close();
    sockets_[socketDescriptor]->deleteLater();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    currentSocket_ = socketDescriptor;

    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
//    connect(socket_, &QTcpSocket::disconnected, this, &Server::onDisconected);
    connect(socket, &QTcpSocket::disconnected, this, [this]{ onDisconected(currentSocket_); });

    sockets_[socketDescriptor] = socket;

    emit sigPrintMsg("New Socket connected \n");
}

} // end namespace dcis::server
