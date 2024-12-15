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
#include <net/client.h>

// APP includes
#include <graph/graph.h>
#include <net/resource.h>
#include <user/userinfo.h>
#include <net/clientsenders.h>

// STL includes

// QT includes
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>

namespace dcis::client
{

Client::Client(LoggerPtrType loggerWidget, QObject *parent)
    : loggerWidget_{loggerWidget}, QObject(parent)
{
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

Client::~Client()
{
    socket_->close();
    socket_->deleteLater();
}

void Client::handle(const Client::HeaderType &header, const QByteArray &body)
{
    loggerWidget_->appendText("********* Packet received *******\n");
    loggerWidget_->appendText("Pakcet size: " + QString::number(common::resource::GLOBAL_HEADER_SIZE + body.size()) +
                  " bytes \nCommand: " + header.command_ + " \nResourceType: " + header.resourceType_ + "\nStatus: " + header.status_ + "\n");
    loggerWidget_->appendText("*********************************\n");

    auto handler = packetHandlerFactory_->createHandler(header.resourceType_);
    if (handler)
    {
        handler->handlePacket(header, body);
    }
    else
    {
        loggerWidget_->appendText("No handler found for " + header.resourceType_ + "\n");
    }
}

bool Client::connectToServer(const QString &ip, const QString &port)
{
    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::onDisconected);

    socket_->connectToHost(ip, port.toInt());

    if (socket_->waitForConnected())
    {
        loggerWidget_->appendText("Succsesfully connected to " + ip + ":" + port + "\n");
        return true;
    }
    else
    {
        loggerWidget_->appendText("Error to connected to " + ip + ":" + port + "\n");
        socket_->close();
        socket_->deleteLater();
        return false;
    }
}

bool Client::checkServerConnected() const
{
    if (socket_ == nullptr)
    {
        return false;
    }

    return socket_->state() == QAbstractSocket::ConnectedState ? true : false;
}

QString Client::getUserName() const
{
    return username_;
}

QString Client::getPassword() const
{
    return password_;
}

void Client::setUserName(const QString userName)
{
    username_ = userName;
}

Client::LoggerPtrType Client::getLogger() const
{
    return loggerWidget_;
}

void Client::setPassword(const QString password)
{
    password_ = password;
}

QTcpSocket *Client::getSocket() const
{
    return socket_;
}

void Client::onReadyRead()
{
    QByteArray buffer;
    QTcpSocket *socket = reinterpret_cast<QTcpSocket *>(sender());

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        loggerWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::GLOBAL_HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        Client::HeaderType header;
        ds >> header;
        handle(header, buffer.mid(common::resource::GLOBAL_HEADER_SIZE));
    }
    else
    {
        loggerWidget_->appendText("Reading from server...\n");
    }
}

void Client::onDisconected()
{
    loggerWidget_->appendText("Server disconected.\n");
}

} // end namespace dcis::client
