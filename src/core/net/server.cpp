/*
 * Project Name: DCIS - Drone Collective Intelligence System
 * Copyright (C) 2022 Artyom Lazyan, Agit Atashyan, Davit Hayrapetyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <net/server.h>

// App includes
#include <graph/graph.h>
#include <net/resource.h>
#include <utils/imageprovider.h>

// Qt includes
#include <QDir>
#include <QThread>
#include <QImageReader>

// STL includes

namespace dcis::core
{

Server::Server(common::utils::ILogger *terminalWidget, QObject *parent)
    : terminalWidget_(terminalWidget), QTcpServer(parent) 
{
    QDir dir;
    const QString workingDir = dir.absolutePath() + "/uploadedImages";
    imageProvider_ = std::make_shared<utils::ImageProvider>();
    imageProvider_->setWorkingDirectoryPath(workingDir);

    packetHandlerFactory_ = std::make_unique<common::resource::PacketHandlerFactory>();

    // register handlers
    packetHandlerFactory_->registerHandler(common::resource::type::Text, [this]() { return std::make_unique<TextPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Json, [this]() { return std::make_unique<JsonPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Command,
                    [this]() { return std::make_unique<CommandPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Attachment,
                    [this]() { return std::make_unique<AttachmentPacketHandler>(this); });
}

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

    delete graphProc_;
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

bool Server::publishWeb(const QByteArray &data)
{
    for (const auto &[client, socket] : clientMap_.asKeyValueRange())
    {
        if (client.isStream && socket->state() == QAbstractSocket::ConnectedState)
        {
            QThread::msleep(100);
            socket->write(data);
            socket->flush();
        }
    }

    return true;
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

void Server::setCurrentSocket(qintptr socketDescriptor)
{ 
    currentSocket_ = socketDescriptor; 
}

int Server::getClientsCount() const
{
    return clientMap_.count();
}

qintptr Server::getCurrentSocket() const 
{ 
    return currentSocket_; 
}

common::utils::ILogger *Server::getTerminalWidget() const 
{
    return terminalWidget_;
}

std::shared_ptr<utils::ImageProvider> Server::getImageProvider() const
{
    return imageProvider_;
}

void Server::handle(const common::resource::Header &header, const QByteArray &body)
{
    terminalWidget_->appendText("********* Packet received *******\n");
    terminalWidget_->appendText("Pakcet size: " + QString::number(common::resource::GLOBAL_HEADER_SIZE + body.size()) +
                  " bytes \nCommand: " + header.command_ + " \nResourceType: " + header.resourceType_ + "\nStatus: " + header.status_ + "\n");
    terminalWidget_->appendText("*********************************\n");

    auto handler = packetHandlerFactory_->createHandler(header.resourceType_);
    if (handler)
    {
        handler->handlePacket(header, body);
    }
    else
    {
        terminalWidget_->appendText("No handler found for " + header.resourceType_ + "\n");
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

} // end namespace dcis::core
