#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <net/resource.h>

// Qt includes
#include <QDir>
#include <QThread>
#include <QImageReader>

// STL includes
#include <iostream>


namespace dcis::server {

Server::Server(common::utils::TerminalWidget* terminalWidget, QObject* parent)
    : terminalWidget_(terminalWidget), QTcpServer(parent)
{
}

Server::~Server()
{
}

void Server::addClient(common::user::UserInfo userInfo)
{
    QTcpSocket* socket = clientMap_.value(common::user::UserInfo { currentSocket_ });
    clientMap_.remove(common::user::UserInfo { currentSocket_ });

    userInfo.socketDescriptor = currentSocket_;
    clientMap_[userInfo] = socket;

    QString msg = "Client:" + userInfo.name + " connected\n";
    terminalWidget_->appendText(msg);
}

bool Server::run(const int port)
{
    if (listen(QHostAddress::Any, port))
    {
        terminalWidget_->appendText("Starting server, listening to " + QString::number(port) + " port\n");
        return true;
    }
    else
    {
        terminalWidget_->appendText("Filed to start server at " + QString::number(port) + " port\n");
        return false;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    currentSocket_ = socketDescriptor;

    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(currentSocket_);
    socket->setSocketOption(QAbstractSocket:: KeepAliveOption, 1);

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconected);

    common::user::UserInfo userInfo;
    userInfo.socketDescriptor = socketDescriptor;

    clientMap_[userInfo] = socket;

    // Send cmd to get info about client
    sendCommand(common::resource::Command::ServerGetUserInfo, socketDescriptor);
}

bool Server::publish(const QByteArray& data, qintptr socketDesc)
{
    QTcpSocket* socket = clientMap_.value(common::user::UserInfo{ socketDesc });

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::DATASTREAM_VERSION);

    socketStream << data;

    QThread::msleep(100);
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        if (socket->waitForBytesWritten())
        {
            terminalWidget_->appendText("Succsesfully sent to server" + QString::number(data.size()) + " bytes\n");
        }
        else
        {
            terminalWidget_->appendText("Error to send to client\n");
            return false;
        }
        socket->flush();
    }

    return true;
}

bool Server::publishAll(const QByteArray& data, QSet<qintptr> excludeSockets)
{
    for (const auto& [client, socket] : clientMap_.asKeyValueRange())
    {
        if (!excludeSockets.contains(client.socketDescriptor))
        {
            publish(data, client.socketDescriptor);
        }
    }

    return true;
}

void Server::sendText(const QString& text, common::resource::Command cmd, qintptr socketDescriptor)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Text;
    header.command = cmd;

    QByteArray resource = text.toUtf8();
    header.bodySize = resource.size();

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);

    resource.prepend(headerData);
    publish(resource, socketDescriptor);
}

void Server::sendJson(const QJsonDocument& json, common::resource::Command cmd, qintptr socketDescriptor)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Json;
    header.command = cmd;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);

    QByteArray resource = json.toJson();
    header.bodySize = resource.size();

    resource.prepend(headerData);
    publish(resource, socketDescriptor);
}

void Server::sendAttachment(const QString& filePath, common::resource::Command cmd, qintptr socketDescriptor)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        common::resource::Header header;
        header.resourceType = common::resource::ResourceType::Attachment;
        header.command = cmd;
        header.fileName = fileName;

        QByteArray headerData;
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        ds << header;

        // header size 128 bytes
        headerData.resize(common::resource::Header::HEADER_SIZE);

        QByteArray resource = file.readAll();

        header.bodySize = resource.size();

        resource.prepend(headerData);
        publish(resource, socketDescriptor);
    }
    else
    {
        terminalWidget_->appendText("Can't open file!\n");
        return;
    }
}

void Server::sendCommand(const common::resource::Command cmd, qintptr socketDescriptor)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Command;
    header.command = cmd;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);
    publish(headerData, socketDescriptor);
}

// slots
void Server::onReadyRead()
{
    QByteArray buffer;
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    currentSocket_ = socket->socketDescriptor();

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::DATASTREAM_VERSION);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        terminalWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::Header::HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        common::resource::Header header;
        ds >> header;
        handle(header, buffer.mid(common::resource::Header::HEADER_SIZE));
    }
    else
    {
        /*terminalWidget_->appendText("Reading from client: " +
                                              QString::number(socket->socketDescriptor()) +
                                              "  Bytes recived: " + QString::number(socket->bytesAvailable()) + " bytes\n");*/
    }
}

void Server::onDisconected()
{
    updateSockets();
}

void Server::handle(const common::resource::Header& header, const QByteArray& body)
{
    QString msg = "Success: data size is " + QString::number(body.size() + 128) + " bytes, Command: " + header.commandToString() + " ResourceType: " + header.typeToString();
    terminalWidget_->appendText(msg);

    header_ = header;

    switch (header.resourceType)
    {
        case common::resource::ResourceType::Text:
        {
            handleText(body);
            break;
        }

        case common::resource::ResourceType::Json:
        {
            handleJson(body);
            break;
        }

        case common::resource::ResourceType::Attachment:
        {
            handleAttachment(body);
            break;
        }

        case common::resource::ResourceType::Command:
        {
            handleCommand(header.command);
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
    terminalWidget_->appendText("Unknown message, doing nothing.\n");
}

void Server::handleAttachment(const QByteArray& data)
{
    QDir dir;
    QString WORKING_DIR = dir.absolutePath() + UPLOADED_IMAGES_PATH;
    dir.mkdir(WORKING_DIR);

    CURRENT_IMAGE_PATH = WORKING_DIR + "/" + header_.fileName;

    QFile file(CURRENT_IMAGE_PATH);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data);
    }

    QImageReader imgReader(CURRENT_IMAGE_PATH);
    if (imgReader.canRead())
    {
        QImage img = imgReader.read();
        QString msg = "IMG width: " + QString::number(img.width()) + " height: " + QString::number(img.height()) + "\n";
        terminalWidget_->appendText(msg);

        imgW_ = img.width();
        imgH_ = img.height();
    }

    QString msg = "File saved at " + WORKING_DIR;
    terminalWidget_->appendText(msg);

    switch (header_.command)
    {
        case common::resource::Command::ServerPublish:
        {
            terminalWidget_->appendText("Sending Image to clients\n");

            common::resource::Header header;
            header.resourceType = common::resource::ResourceType::Attachment;
            header.command = common::resource::Command::ClientShowImage;
            header.fileName = header_.fileName;

            QByteArray headerData;
            QDataStream ds(&headerData, QIODevice::ReadWrite);
            ds << header;

            // header size 128 bytes
            headerData.resize(common::resource::Header::HEADER_SIZE);

            QByteArray resource = data;
            header.bodySize = data.size();

            resource.prepend(headerData);
            publishAll(resource, QSet<qintptr>{ currentSocket_ });
            break;
        }
        default:
            return;
    }
}

void Server::handleText(const QByteArray& data)
{
    QString msg = "Recived Text: " + data + "\n";
    terminalWidget_->appendText(msg);

    switch (header_.command)
    {
        case common::resource::Command::ServerPrintText:
        {
            terminalWidget_->appendText(QString::fromUtf8(data));
            break;
        }
        default:
            return;
    }
}

void Server::handleJson(const QByteArray& data)
{
    terminalWidget_->appendText("Recived Json\n");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if(commGraph_)
    {
        delete commGraph_;
    }
    commGraph_ = GraphProcessor::commonGraph::fromJSON(jsonDocument);

    terminalWidget_->appendText("TOP_LEFT: " + commGraph_->getLeftTop() + "\n");
    terminalWidget_->appendText("BOTTOM_RIGHT: " + commGraph_->getRightBottom() + "\n");

    switch (header_.command)
    {
        case common::resource::Command::ServerPublish:
        {
            terminalWidget_->appendText("Sending JSON to clients\n");

            common::resource::Header header;
            header.resourceType = common::resource::ResourceType::Json;
            header.command = common::resource::Command::ClientUpdateGraph;

            QByteArray headerData;
            QDataStream ds(&headerData, QIODevice::ReadWrite);
            ds << header;

            // header size 128 bytes
            headerData.resize(common::resource::Header::HEADER_SIZE);

            QByteArray resource = data;
            header.bodySize = data.size();

            resource.prepend(headerData);
            publishAll(resource, QSet<qintptr>{ currentSocket_ });
            break;
        }
        case common::resource::Command::ClientSetUserInfo:
        {
            QJsonDocument json = QJsonDocument::fromJson(data);
            common::user::UserInfo userInfo = common::user::UserInfo::fromJson(json);
            addClient(userInfo);

            sendCommand(common::resource::Command::ServerUserAccepted, currentSocket_);
            break;
        }
        default:
            return;
        }
}

void Server::handleCommand(const common::resource::Command cmd)
{
    switch (cmd)
    {
        case common::resource::Command::ClientClearCycles:
        {
            // clear cycles
            terminalWidget_->appendText("ClearCycles\n");

            if(graphProc_)
            {
                delete graphProc_;
            }
            graphProc_ = new dcis::GraphProcessor::GraphProcessor();
            graphProc_->setCommGraph(commGraph_);
            graphProc_->initGraph();
            graphProc_->initGraphDirs();
            graphProc_->clearCycles();
            break;
        }
        case common::resource::Command::ClientGenerateGraph:
        {
            if(graphProc_)
            {
                terminalWidget_->appendText("GenereateGraph\n");

                graphProc_->setImgSize(imgW_, imgH_);
                graphProc_->generateGraph();
            }
            break;
        }
        case common::resource::Command::ClientStartExploration:
        {
            if(graphProc_)
            {
                terminalWidget_->appendText("StartExploration\n");

                graphProc_->startExploration();
            }
            break;
        }
        case common::resource::Command::ClientStartAttack:
        {
            terminalWidget_->appendText("StartAttack\n");

            // TO DO: start attack
            break;
        }
        default:
            return;
    }

}

void Server::syncSockets()
{
    // Send image
/*    {
        QFile file(CURRENT_IMAGE_PATH);
        if (file.open(QIODevice::ReadOnly))
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

            QByteArray resource = file.readAll();
            header.bodySize = resource.size();

            resource.prepend(headerData);

            for (auto& socket : sockets_)
            {
                QDataStream socketStream(socket);
                socketStream.setVersion(resource::DATASTREAM_VERSION);

                socketStream << resource;

                QThread::msleep(200);
                if(socket->state() == QAbstractSocket::ConnectedState)
                {
                    if (socket->waitForBytesWritten())
                    {
                        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Succsesfully sent to server" + QString::number(resource.size()) + " bytes");
                        continue;
                    }
                    else
                    {
                        utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to send to server");
                        continue;
                    }
                }
            }
        }

    }
*/
    // Sent Graph
/*    {
        QJsonDocument json = GraphProcessor::commonGraph::toJSON(commGraph_);
        QByteArray data = json.toJson();
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

        for (auto& socket : sockets_)
        {
            QDataStream socketStream(socket);
            socketStream.setVersion(resource::DATASTREAM_VERSION);

            socketStream << resource;

            QThread::msleep(500);
            if(socket->state() == QAbstractSocket::ConnectedState)
            {
                if (socket->waitForBytesWritten())
                {
                    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Succsesfully sent to server" + QString::number(resource.size()) + " bytes");
                    continue;
                }
                else
                {
                    utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to send to server");
                    continue;
                }
            }
        }
    }*/
}

void Server::updateSockets()
{
    // Iterate through the map in reverse order and remove key-value pairs that meet the criteria
    for (auto it = clientMap_.end(); it != clientMap_.begin();)
    {
        --it; // Move iterator to the previous element

        // Check the condition and remove the key-value pair if needed
        if (!(it.value()->state() == QTcpSocket::ConnectedState))
        {
            QString msg = "Client:" + it.key().name + " disconnected\n";
            it.value()->close();
            it.value()->deleteLater();
            it = clientMap_.erase(it); // Erase the current element and get the next iterator
            terminalWidget_->appendText(msg);
        }
    }
}

} // end namespace dcis::server
