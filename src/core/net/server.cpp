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
#include <net/serversenders.h>
#include <utils/imageprovider.h>

// Qt includes
#include <QDir>
#include <QThread>
#include <QImageReader>

// STL includes

namespace dcis::core
{

Server::Server(LoggerPtrType loggerWidget, QObject *parent)
    : loggerWidget_(loggerWidget), QTcpServer(parent) 
{
    QDir dir;
    const QString workingDir = dir.absolutePath() + "/uploadedImages";
    imageProvider_ = std::make_shared<utils::ImageProvider>();
    imageProvider_->setWorkingDirectoryPath(workingDir);

    packetHandlerFactory_ = std::make_unique<common::resource::PacketHandlerFactory>();

    // register handlers
    packetHandlerFactory_->registerHandler(common::resource::type::Text, 
                                        [this]() { return std::make_unique<TextPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Json, 
                                        [this]() { return std::make_unique<JsonPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Command,
                                        [this]() { return std::make_unique<CommandPacketHandler>(this); });
    packetHandlerFactory_->registerHandler(common::resource::type::Attachment,
                                        [this]() { return std::make_unique<AttachmentPacketHandler>(this); });
}

Server::~Server() {}

void Server::addClient(UserInfoType userInfo)
{
    for (const auto &[client, socket] : clientMap_.asKeyValueRange())
    {
        if (client.name == userInfo.name)
        {
            StatusUpdateSender sender(currentSocket_, this);
            sender.send(common::resource::status::UserAlreadyConnected);
            return;
        }
    }

    QTcpSocket *socket = clientMap_.value(common::user::UserInfo{currentSocket_});
    clientMap_.remove(common::user::UserInfo{currentSocket_});

    userInfo.socketDescriptor = currentSocket_;
    clientMap_[userInfo] = socket;

    loggerWidget_->appendText("Client:" + userInfo.name + " connected\n");

    delete graphProc_;
}

bool Server::run(const int port)
{
    if (listen(QHostAddress::Any, port))
    {
        loggerWidget_->appendText("Starting server, listening to " + QString::number(port) + " port\n");
        return true;
    }
    else
    {
        loggerWidget_->appendText("Filed to start server at " + QString::number(port) + " port\n");
        return false;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    loggerWidget_->appendText("New incoming connection ... \n");

    currentSocket_ = socketDescriptor;

    QTcpSocket *socket = new QTcpSocket();
    socket->setSocketDescriptor(currentSocket_);
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconected);

    UserInfoType userInfo;
    userInfo.socketDescriptor = socketDescriptor;

    clientMap_[userInfo] = socket;

    // Send cmd to get info about client
    CommandSender sender(socketDescriptor, this);
    sender.send(common::resource::command::server::GetUserInfo);
}

// slots
void Server::onReadyRead()
{
    QByteArray buffer;
    QTcpSocket *socket = reinterpret_cast<QTcpSocket *>(sender());

    currentSocket_ = socket->socketDescriptor();

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        loggerWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::GLOBAL_HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        common::resource::Header header;
        ds >> header;
        handle(header, buffer.mid(common::resource::GLOBAL_HEADER_SIZE));
    }
    else
    {
        loggerWidget_->appendText("#");
        /*loggerWidget_->appendText("Reading from client: " +
                                              QString::number(socket->socketDescriptor()) +
                                              "  Bytes recived: " + QString::number(socket->bytesAvailable()) + "
           bytes\n");*/
    }

    // This is for web server, the header size is fixed 65 bytes
    if (socket->bytesAvailable() == 65 && socket->readAll().contains("Stream"))
    {
        loggerWidget_->appendText("New stream connection\n");

        for (const auto &[client, socket] : clientMap_.asKeyValueRange())
        {
            if (socket->socketDescriptor() == currentSocket_)
            {
                QTcpSocket *socket = clientMap_.value(UserInfoType{ currentSocket_ });
                clientMap_.remove(UserInfoType{ currentSocket_ });

                UserInfoType userInfo;
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

Server::ClientMapType Server::getClientMap() const
{
    return clientMap_;
}

qintptr Server::getCurrentSocket() const 
{ 
    return currentSocket_; 
}

Server::LoggerPtrType Server::getLogger() const 
{
    return loggerWidget_;
}

Server::ImageProviderPtrType Server::getImageProvider() const
{
    return imageProvider_;
}

void Server::setCommGraph(Server::CommonGraphType* cg)
{
    commGraph_ = cg;
}

Server::CommonGraphType* Server::getCommGraph() const
{
    return commGraph_;
}
void Server::setGraphProc(Server::GraphProcType* gp)
{
    graphProc_ = gp;
}
Server::GraphProcType* Server::getGraphProc() const
{
    return graphProc_;
}

void Server::handle(const HeaderType &header, const QByteArray &body)
{
    loggerWidget_->appendText("********* Packet received *******\n");
    loggerWidget_->appendText("Pakcet size: " + QString::number(common::resource::GLOBAL_HEADER_SIZE + body.size()) +
                  " bytes \nCommand: " + header.command_ + " \nResourceType: " + header.resourceType_ + "\nStatus: " + header.status_ + "\n");
    loggerWidget_->appendText("*********************************\n");

    auto handler = packetHandlerFactory_->createHandler(header.resourceType_);
    if (handler != nullptr)
    {
        handler->handlePacket(header, body);
    }
    else
    {
        loggerWidget_->appendText("No handler found for " + header.resourceType_ + "\n");
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
            loggerWidget_->appendText(msg);
        }
    }
}

} // end namespace dcis::core
