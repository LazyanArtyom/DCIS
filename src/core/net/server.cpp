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

void Server::onDisconected()
{
    updateSockets();
}

void sendjson(Server* server)
{
// debugstream
    QJsonObject jsonObj;

    //insert single datas first
    jsonObj.insert("directed", true);

    // fill nodes
    QJsonArray nodes;
    QJsonObject jsonNode;
    jsonNode.insert("name", "Node1");
    jsonNode.insert("x", 1.1);
    jsonNode.insert("y", 11.11);
    nodes.push_back(jsonNode);

    QJsonObject jsonNode2;
    jsonNode2.insert("name", "Node2");
    jsonNode2.insert("x", 2.2);
    jsonNode2.insert("y", 22.22);
    nodes.push_back(jsonNode2);

    QJsonObject jsonNode3;
    jsonNode3.insert("name", "Node3");
    jsonNode3.insert("x", 3.3);
    jsonNode3.insert("y", 33.33);
    nodes.push_back(jsonNode3);

    jsonObj.insert("nodes", nodes);

    // fill edges
    QJsonArray edges;

    QJsonObject jsonEdge;
    jsonEdge.insert("start", "Node1");
    jsonEdge.insert("end", "Node2");
    edges.push_back(jsonEdge);

    QJsonObject jsonEdge2;
    jsonEdge2.insert("start", "Node2");
    jsonEdge2.insert("end", "Node3");
    edges.push_back(jsonEdge2);

    QJsonObject jsonEdge3;
    jsonEdge3.insert("start", "Node3");
    jsonEdge3.insert("end", "Node1");
    edges.push_back(jsonEdge3);

    jsonObj.insert("edges", edges);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    resource::Body body(jsonDoc);
    resource::Header header(sizeof(body), resource::Command::Publish, resource::ResourceType::Json);

    server->publish(header, body);

    utils::log(utils::LogLevel::INFO, "sending json to client");
    utils::log(utils::LogLevel::INFO, jsonDoc.toJson().toStdString());
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    currentSocket_ = socketDescriptor;

    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    socket->setSocketOption(QAbstractSocket:: KeepAliveOption, 1);

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconected);

    sockets_[socketDescriptor] = socket;

    std::string msg = "Client:" + std::to_string(socketDescriptor) + " connected.";
    utils::log(utils::LogLevel::ERROR, msg);

    sendjson(this);
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

void Server::updateSockets()
{
    for (auto socket = sockets_.begin(); socket != sockets_.end();)
    {
        if (!(socket.value()->state() == QTcpSocket::ConnectedState))
        {
            std::string msg = "Client:" + std::to_string(socket.key()) + " disconnected.";
            utils::log(utils::LogLevel::ERROR, msg);

            socket.value()->close();
            socket.value()->deleteLater();
            sockets_.erase(socket++);
        }
        else
        {
            ++socket;
        }
    }
}

} // end namespace dcis::server
