#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <net/resource.h>

// Qt includes
#include <QDir>
#include <QThread>
#include <QImageReader>

// STL includes

namespace dcis::server
{

Server::Server(common::utils::ILogger *terminalWidget, QObject *parent)
    : terminalWidget_(terminalWidget), QTcpServer(parent) {}

Server::~Server() {}

void Server::addClient(common::user::UserInfo userInfo)
{
    for (const auto &[client, socket] : clientMap_.asKeyValueRange())
    {
        if (client.name == userInfo.name)
        {
            sendStatusUpdate(common::resource::status::UserAlreadyConnected, currentSocket_);
            return;
        }
    }

    QTcpSocket *socket = clientMap_.value(common::user::UserInfo{currentSocket_});
    clientMap_.remove(common::user::UserInfo{currentSocket_});

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
    terminalWidget_->appendText("New incoming connection ... \n");

    currentSocket_ = socketDescriptor;

    QTcpSocket *socket = new QTcpSocket();
    socket->setSocketDescriptor(currentSocket_);
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconected);

    common::user::UserInfo userInfo;
    userInfo.socketDescriptor = socketDescriptor;

    clientMap_[userInfo] = socket;

    // Send cmd to get info about client
    sendCommand(common::resource::command::server::GetUserInfo, socketDescriptor);
}

bool Server::publish(const QByteArray &data, qintptr socketDesc)
{
    QTcpSocket *socket = clientMap_.value(common::user::UserInfo{socketDesc});

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);

    socketStream << data;

    QThread::msleep(100);
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        if (socket->waitForBytesWritten())
        {
            terminalWidget_->appendText("Succsesfully sent to client" + QString::number(data.size()) + " bytes\n");
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

bool Server::publishAll(const QByteArray &data, QSet<qintptr> excludeSockets)
{
    for (const auto &[client, socket] : clientMap_.asKeyValueRange())
    {
        if (!excludeSockets.contains(client.socketDescriptor) && !client.isStream)
        {
            publish(data, client.socketDescriptor);
        }
    }

    return true;
}

void Server::sendText(const QString &text, const QString cmd, qintptr socketDescriptor)
{
    QByteArray body = text.toUtf8();
    publish(common::resource::create(cmd, 
                                     common::resource::type::Text, 
                                     common::resource::status::Ok, 
                                     body), 
                                     socketDescriptor);
}

void Server::sendJson(const QJsonDocument &json, const QString cmd, qintptr socketDescriptor)
{
    QByteArray body = json.toJson();
    publish(common::resource::create(cmd, 
                                     common::resource::type::Json, 
                                     common::resource::status::Ok, 
                                     body), 
                                     socketDescriptor);
}

void Server::sendAttachment(const QString &filePath, const QString cmd, qintptr socketDescriptor)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        QByteArray body = file.readAll();
        publish(common::resource::create(cmd, 
                                         common::resource::type::Attachment, 
                                         common::resource::status::Ok, 
                                         body, 
                                         fileName), 
                                         socketDescriptor);
    }
    else
    {
        terminalWidget_->appendText("Can't open file!\n");
        return;
    }
}

void Server::sendCommand(const QString cmd, qintptr socketDescriptor)
{
    publish(common::resource::create(cmd, 
                                     common::resource::type::Command), 
                                     socketDescriptor);
}


void Server::sendStatusUpdate(const QString status, qintptr socketDescriptor)
{
    publish(common::resource::create(common::resource::command::StatusUpdate, 
                                     common::resource::type::Command, 
                                     status),
                                     socketDescriptor);
}

// slots
void Server::onReadyRead()
{
    terminalWidget_->appendText("ON READY READ\n");
    QByteArray buffer;
    QTcpSocket *socket = reinterpret_cast<QTcpSocket *>(sender());

    currentSocket_ = socket->socketDescriptor();

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        terminalWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::GLOBAL_HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        common::resource::Header header;
        ds >> header;
        handle(header, buffer.mid(common::resource::GLOBAL_HEADER_SIZE));
    }
    else
    {
        terminalWidget_->appendText("#");
        /*terminalWidget_->appendText("Reading from client: " +
                                              QString::number(socket->socketDescriptor()) +
                                              "  Bytes recived: " + QString::number(socket->bytesAvailable()) + "
           bytes\n");*/
    }

    // This is for web server, the header size is fixed 65 bytes
    if (socket->bytesAvailable() == 65 && socket->readAll().contains("Stream"))
    {
        terminalWidget_->appendText("New stream connection\n");

        for (const auto &[client, socket] : clientMap_.asKeyValueRange())
        {
            if (socket->socketDescriptor() == currentSocket_)
            {
                terminalWidget_->appendText("Adding to user Map\n");
                QTcpSocket *socket = clientMap_.value(common::user::UserInfo{currentSocket_});
                clientMap_.remove(common::user::UserInfo{currentSocket_});

                common::user::UserInfo userInfo;
                userInfo.socketDescriptor = currentSocket_;
                userInfo.isStream = true;
                clientMap_[userInfo] = socket;
                break;
            }
        }
        return;
    }
}

void Server::onDisconected()
{
    updateSockets();
}

void Server::handle(const common::resource::Header &header, const QByteArray &body)
{
    terminalWidget_->appendText("Success: data size is " + QString::number(body.size() + 128) +
                  " bytes, Command: " + header.command_ + " ResourceType: " + header.resourceType_ + "\n");

    header_ = header;

    if (header.resourceType_ == common::resource::type::Text)
    {
        handleText(body);
    }
    else if (header.resourceType_ == common::resource::type::Json)
    {
        handleJson(body);
    }
    else if (header.resourceType_ == common::resource::type::Attachment)
    {
        handleAttachment(body);
    }
    else if (header.resourceType_ == common::resource::type::Command)
    {
        handleCommand(header.command_);
    }
    else {
        handleUnknown();
    }
}

void Server::handleUnknown()
{
    terminalWidget_->appendText("Unknown message, doing nothing.\n");
}

void Server::handleAttachment(const QByteArray &data)
{
    QDir dir;
    QString WORKING_DIR = dir.absolutePath() + UPLOADED_IMAGES_PATH;
    dir.mkdir(WORKING_DIR);

    CURRENT_IMAGE_PATH = WORKING_DIR + "/" + header_.fileName_;

    QFile file(CURRENT_IMAGE_PATH);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
    }

    QImageReader imgReader(CURRENT_IMAGE_PATH);
    if (imgReader.canRead())
    {
        QImage img = imgReader.read();
        terminalWidget_->appendText("IMG width: " + QString::number(img.width()) + " height: " + QString::number(img.height()) + "\n");

        imgW_ = img.width();
        imgH_ = img.height();
    }

    QString msg = "File saved at " + WORKING_DIR;
    terminalWidget_->appendText(msg);

    if (header_.command_ == common::resource::command::server::Publish)
    {
        terminalWidget_->appendText("Sending Image to clients\n");

        QByteArray body = data;
        publishAll(common::resource::create(common::resource::command::client::ShowImage, 
                                                 common::resource::type::Attachment, 
                                                 common::resource::status::Ok, body),
                                                 QSet<qintptr>{currentSocket_});

        for (const auto &[client, socket] : clientMap_.asKeyValueRange())
        {
            if (client.isStream && socket->state() == QAbstractSocket::ConnectedState)
            {
                QThread::msleep(100);
                QJsonObject jsonObj;
                jsonObj["imagePath"] = CURRENT_IMAGE_PATH;
                jsonObj["width"] = QString::number(imgW_);
                jsonObj["height"] = QString::number(imgH_);
                QJsonDocument jsonDoc(jsonObj);

                socket->write(jsonDoc.toJson());
                socket->flush();
            }
        }
    }
}

void Server::handleText(const QByteArray &data)
{
    terminalWidget_->appendText("Recived Text: " + data + "\n");

    if (header_.command_ == common::resource::command::PrintText)
    {
        terminalWidget_->appendText(QString::fromUtf8(data));
    }
}

void Server::handleJson(const QByteArray &data)
{
    if (header_.command_ == common::resource::command::server::Publish)
    {
        terminalWidget_->appendText("Recived Json\n");

        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        if (commGraph_)
        {
            delete commGraph_;
        }
        commGraph_ = GraphProcessor::commonGraph::fromJSON(jsonDocument);

        terminalWidget_->appendText("TOP_LEFT: " + commGraph_->getLeftTop() + "\n");
        terminalWidget_->appendText("BOTTOM_RIGHT: " + commGraph_->getRightBottom() + "\n");

        if (clientMap_.count() > 1)
        {
            terminalWidget_->appendText("Sending JSON to clients\n");

            QByteArray body = data;
            publishAll(common::resource::create(common::resource::command::client::UpdateGraph, 
                                                common::resource::type::Json,
                                                common::resource::status::Ok, 
                                                body), 
                                                QSet<qintptr>{currentSocket_});

            for (const auto &[client, socket] : clientMap_.asKeyValueRange())
            {
                if (client.isStream && socket->state() == QAbstractSocket::ConnectedState)
                {
                    QThread::msleep(100);
                    socket->write(data);
                    socket->flush();
                }
            }
        }
    }
    else if (header_.command_ == common::resource::command::server::SetUserInfo)
    {
        QJsonDocument json = QJsonDocument::fromJson(data);
        common::user::UserInfo userInfo = common::user::UserInfo::fromJson(json);

        // TO DO: Create normal login system
        if (userInfo.name == "artyom" && userInfo.password == "al1234" ||
            userInfo.name == "agit"   && userInfo.password == "aa1234" ||
            userInfo.name == "davit"  && userInfo.password == "dh1234" ||
            userInfo.name == "root"   && userInfo.password == "root")
        {
            addClient(userInfo);
            sendStatusUpdate(common::resource::status::UserAccepted, currentSocket_);

            // sync new user
            if (clientMap_.count() > 1)
            {
                sendAttachment(CURRENT_IMAGE_PATH, common::resource::command::client::ShowImage, currentSocket_);

                if (commGraph_ == nullptr)
                {
                    return;
                }

                QJsonDocument json = GraphProcessor::commonGraph::toJSON(commGraph_);
                sendJson(json, common::resource::command::client::UpdateGraph, currentSocket_);
            }
        }
        else
        {
            sendStatusUpdate(common::resource::status::UserDeclined, currentSocket_);
        }
    }
}

void Server::handleCommand(const QString cmd)
{
    if (cmd == common::resource::command::server::ClearCycles)
    {
        terminalWidget_->appendText("ClearCycles\n");

        if (graphProc_)
        {
            delete graphProc_;
        }
        graphProc_ = new dcis::GraphProcessor::GraphProcessor();
        graphProc_->setCommGraph(commGraph_);
        graphProc_->initGraph();
        graphProc_->initGraphDirs();
        graphProc_->clearCycles();

    }
    else if (cmd == common::resource::command::server::GenerateGraph)
    {
        if (graphProc_)
        {
            terminalWidget_->appendText("GenereateGraph\n");

            graphProc_->setImgSize(imgW_, imgH_);
            graphProc_->generateGraph();
        }
    }
    else if (cmd == common::resource::command::server::StartExploration)
    {
        if (graphProc_)
        {
            terminalWidget_->appendText("StartExploration\n");

            graphProc_->startExploration();
        }
    }
    else if (cmd == common::resource::command::server::StartAttack)
    {
        // TO DO: start attack
        terminalWidget_->appendText("StartAttack | TO DO: Implement | \n");
    }
}

void Server::updateSockets()
{
    // Iterate through the map in reverse order and remove key-value pairs that meet the criteria
    for (auto it = clientMap_.end(); it != clientMap_.begin();)
    {
        --it;

        if (!(it.value()->state() == QTcpSocket::ConnectedState))
        {
            QString msg = "Client:" + it.key().name + " disconnected\n";
            it.value()->close();
            it.value()->deleteLater();
            it = clientMap_.erase(it);
            terminalWidget_->appendText(msg);
        }
    }
}

} // end namespace dcis::server
