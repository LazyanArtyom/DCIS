#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <graphprocessor/graphprocessor.h>
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
}

Server::~Server()
{
   // utils::log(utils::LogLevel::INFO, "End Server.");
}

bool Server::run(const int port)
{
    if (listen(QHostAddress::Any, port))
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
    QByteArray buffer;
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    currentSocket_ = socket->socketDescriptor();

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_6_4);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Buffer size: : " + QString::number(buffer.size()) + " bytes");

        QByteArray headerData = buffer.mid(0, resource::Header::HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        resource::Header header;
        ds >> header;
        handle(header, buffer.mid(resource::Header::HEADER_SIZE));
    }
    else
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Reading from client: " +
                                              QString::number(socket->socketDescriptor()) +
                                              "  Bytes recived: " + QString::number(socket->bytesAvailable()) + " bytes");
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

    //resource::Header header(sizeof(body), resource::Command::Publish, resource::ResourceType::Json);

    //server->publish(header, body);
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

bool Server::publish(const QByteArray& data)
{
    for (auto& socket : sockets_)
    {
        if (socket->socketDescriptor() == currentSocket_)
        {
            continue;
        }

        QDataStream socketStream(socket);
        socketStream.setVersion(resource::DATASTREAM_VERSION);

        socketStream << data;

        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            if (socket->waitForBytesWritten())
            {
                utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Succsesfully sent to server" + QString::number(data.size()) + " bytes");
                continue;
            }
            else
            {
                utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to send to server");
                continue;
            }
        }
    }

    return true;
}

void Server::handle(const resource::Header& header, const QByteArray& body)
{
    QString msg = "Success: data size is " + QString::number(body.size() + 128) + " bytes, Command: " + header.commandToString() + " ResourceType: " + header.typeToString();
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);

    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            handleString(body);
            break;
        }

        case resource::ResourceType::Json:
        {
            handleJson(body);
            break;
        }

        case resource::ResourceType::Attachment:
        {
            handleAttachment(body);
            break;
        }

        default:
        {
            handleUnknown();
        }
    }
}

void Server::handleUnknown()
{
    // utils::log(utils::LogLevel::INFO, "Unknown message, doing nothing.");
}

void Server::handleAttachment(const QByteArray& data)
{
    QDir dir;
    QString WORKING_DIR = dir.absolutePath() + UPLOADED_IMAGES_PATH;
    dir.mkdir(WORKING_DIR);
    QFile file(WORKING_DIR + "/" + header_.fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data);
    }

    QString msg = "File saved at " + WORKING_DIR;
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Sending Image to clients");
            resource::Header header;
            header.resourceType = resource::ResourceType::Attachment;
            header.command = resource::Command::ShowImage;
            header.fileName = header_.fileName;

            QByteArray headerData;
            QDataStream ds(&headerData, QIODevice::ReadWrite);
            ds << header;

            // header size 128 bytes
            headerData.resize(resource::Header::HEADER_SIZE);

            QByteArray resource = data;
            header.bodySize = data.size();

            resource.prepend(headerData);
            publish(resource);
            break;
        }
        default:
            return;
    }
}

void Server::handleString(const QByteArray& data)
{
    QString msg(data);
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Recived message: " + msg);

    switch (header_.command)
    {
        case resource::Command::ClearCycles:
        {
            // clear cycles
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, "ClearCycles");
            break;
        }
        default:
            return;
    }

}

void Server::handleJson(const QByteArray& data)
{
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Recived Json");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    GraphProcessor::commonGraph* graph = GraphProcessor::commonGraph::fromJSON(jsonDocument);




    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Sending JSON to clients");
            resource::Header header;
            header.resourceType = resource::ResourceType::Json;
            header.command = resource::Command::UpdateGraph;

            QByteArray headerData;
            QDataStream ds(&headerData, QIODevice::ReadWrite);
            ds << header;

            // header size 128 bytes
            headerData.resize(resource::Header::HEADER_SIZE);

            QByteArray resource = data;
            header.bodySize = data.size();

            resource.prepend(headerData);
            publish(resource);
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
