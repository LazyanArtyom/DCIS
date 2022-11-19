#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <net/resource.h>

// Qt includes

// STL includes
#include <iostream>


namespace dcis::server {

// public
Server::Server(QObject* parent)
    : QTcpServer(parent)
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

            resource::Body body;
            input >> body;

            nextBlockSize_ = 0;
            handle(head, body);
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

void Server::publish(resource::Header header, resource::Body body)
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

void Server::handle(resource::Header header, resource::Body body)
{
    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            handleJson(body.data.toJsonDocument());
            break;
        }

        default:
            handleUnknown();
    }
}

void Server::handleUnknown()
{
    utils::log(utils::LogLevel::INFO, "Unknown message, doing nothing.");
}

void Server::handleString(const QString str)
{
    utils::log(utils::LogLevel::INFO, "String passed: ", str.toStdString());

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Publishing string to others.");
            publish(header_, resource::Body(str));
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }

}

void Server::handleJson(const QJsonDocument json)
{
    utils::log(utils::LogLevel::INFO, "Json passed: ", json.toJson().toStdString());

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Publishing JSON to others.");
            publish(header_, resource::Body(json));
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }
}

} // end namespace dcis::server
