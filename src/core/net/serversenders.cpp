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
#include <net/serversenders.h>

// APP includes
#include <net/server.h>

// QT includes
#include <QDir>
#include <QThread>
#include <QTcpSocket>

namespace dcis::core
{

SenderBase::SenderBase(const qintptr socketDesc, Server *server) 
    : socketDescriptor_(socketDesc), server_(server) {}

bool SenderBase::publishWeb(const QByteArray &data)
{
    for (const auto &[client, socket] : server_->getClientMap().asKeyValueRange())
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

bool SenderBase::publish(const qintptr socketDesc, const QByteArray &data)
{
    QTcpSocket *socket = server_->getClientMap().value(Server::UserInfoType{socketDesc}, nullptr);
    if (socket == nullptr)
    {
        return false;
    }

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);

    socketStream << data;

    QThread::msleep(100);
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        if (socket->waitForBytesWritten())
        {
            server_->getLogger()->appendText("Succsesfully sent to client" + QString::number(data.size()) + " bytes\n");
        }
        else
        {
            server_->getLogger()->appendText("Error to send to client\n");
            return false;
        }
        socket->flush();
    }

    return true;
}

bool SenderBase::publishAll(const QByteArray &data, QSet<qintptr> excludeSockets)
{
    for (const auto &[client, socket] : server_->getClientMap().asKeyValueRange())
    {
        if (!excludeSockets.contains(client.socketDescriptor) && !client.isStream)
        {
            publish(client.socketDescriptor, data);
        }
    }

    return true;
}

TextSender::TextSender(const qintptr socketDesc, Server *server)
    : SenderBase(socketDesc, server) {}

void TextSender::send(const QString &text, const QString cmd)
{
    publish(socketDescriptor_, 
                  common::resource::create(cmd,
                                           common::resource::type::Text, 
                                         common::resource::status::Ok, 
                                           text.toUtf8()));
}

AttachmentSender::AttachmentSender(const qintptr socketDesc, Server *server)
    : SenderBase(socketDesc, server) {}

void AttachmentSender::send(const QString &filePath, const QString cmd)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        publish(socketDescriptor_, 
                      common::resource::create(cmd, 
                                               common::resource::type::Attachment, 
                                             common::resource::status::Ok, 
                                               file.readAll(), 
                                                     fileName));
    }
    else
    {
        server_->getLogger()->appendText("Can't open file!\n");
        return;
    }
}

JsonSender::JsonSender(const qintptr socketDesc, Server *server)
    : SenderBase(socketDesc, server) {}

void JsonSender::send(const QJsonDocument &json, const QString cmd)
{
    publish(socketDescriptor_, 
                  common::resource::create(cmd, 
                                           common::resource::type::Json, 
                                         common::resource::status::Ok, 
                                           json.toJson()));
}

CommandSender::CommandSender(const qintptr socketDesc, Server *server) 
        : SenderBase(socketDesc, server) {}

void CommandSender::send(const QString cmd)
{
    publish(socketDescriptor_, 
                  common::resource::create(cmd, 
                                           common::resource::type::Command,
                                         common::resource::status::Ok));
}

StatusUpdateSender::StatusUpdateSender(const qintptr socketDesc, Server *server) 
        : SenderBase(socketDesc, server) {}

void StatusUpdateSender::send(const QString status)
{
    publish(socketDescriptor_, 
            common::resource::create(common::resource::command::StatusUpdate, 
                                          common::resource::type::Command, 
                                                status));
}

} // end namespace dcis::core
