#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <net/resource.h>
#include <utils/debugstream.h>

// Qt includes
#include <QDir>

// STL includes
#include <iostream>


namespace dcis::server {

// public
Server::Server(QObject* parent)
    : QTcpServer(parent)
{
    nextBlockSize_ = 0;
    run(2323);
}

Server::~Server()
{
   // utils::log(utils::LogLevel::INFO, "End Server.");
}

bool Server::run(const int port)
{
    if (listen(QHostAddress::Any, 2323))
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Starting server, listening to " + QString::number(port) + " port");
        return true;
    }
    else
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Filed to start server at " + QString::number(port) + " port");
        return false;
    }
}

// slots
void Server::onReadyRead()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Reading from client: " + QString::number(socket->socketDescriptor()));

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
        utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to read, bad package");
    }
}

void Server::onDisconected()
{
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Server disconected");
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

    QString msg = "Client:" + QString::number(socketDescriptor) + " connected";
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
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
    QString msg = "Success: data size is " + QString::number(header.bodySize) + " bytes, Command: " + header_.commandToString() + "DataType: ";

    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            msg += "STRING";
            handleString(body.data.toString());
            break;
        }

        case resource::ResourceType::Json:
        {
            msg += "JSON";
            handleJson(body.data.toJsonDocument());
            break;
        }

        case resource::ResourceType::Image:
        {
            msg += "IMAGE";
            handleImage(body.data.value<QImage>());
            break;
        }

        default:
        {
            msg += "UNKNOWN";
            handleUnknown();
        }
    }

    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
}

void Server::handleUnknown()
{
    // utils::log(utils::LogLevel::INFO, "Unknown message, doing nothing.");
}

void Server::handleImage(const QImage& img)
{
    QDir dir;
    QString WORKING_DIR = dir.absolutePath() + UPLOADED_IMAGES_PATH;
    dir.mkdir(WORKING_DIR);
    img.save(WORKING_DIR + "/test.png");

    QString msg = "Image saved at " + WORKING_DIR;
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
}

void Server::handleString(const QString& str)
{
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            publish(header_, resource::Body(str));
            break;
        }
        default:
            return;
    }

}

void Server::handleJson(const QJsonDocument& json)
{
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            publish(header_, resource::Body(json));
            break;
        }
        default:
            return;
    }
}

void Server::updateSockets()
{
    for (auto socket = sockets_.begin(); socket != sockets_.end();)
    {
        if (!(socket.value()->state() == QTcpSocket::ConnectedState))
        {
            QString msg = "Client:" + QString::number(socket.key()) + " disconnected.";
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);

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
