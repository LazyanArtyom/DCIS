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
#include <net/clientsenders.h>

// APP includes
#include <net/client.h>

// QT includes
#include <QDir>
#include <QThread>
#include <QTcpSocket>
#include <QJsonDocument>

namespace dcis::client
{

SenderBase::SenderBase(QTcpSocket *socket, Client *client) 
    : socket_(socket), client_(client) {}

bool SenderBase::send(const QByteArray &data)
{
    if (!client_->checkServerConnected())
    {
        client_->getLogger()->appendText("Server does not respond. Please reconnect!\n");
        return false;
    }
    
    if (socket_->state() != QAbstractSocket::ConnectedState)
    {
        client_->getLogger()->appendText("Socket is not in connected state.\n");
        return false;
    }

    QDataStream socketStream(socket_);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);

    socketStream << data;

    if (socket_->state() == QAbstractSocket::ConnectedState)
    {
        QThread::msleep(100);
        if (socket_->waitForBytesWritten())
        {
            client_->getLogger()->appendText("Data sent successfully.\n");
        }
        else
        {
            client_->getLogger()->appendText("Failed to send data.\n");
            return false;
        }
        socket_->flush();
    }
    else
    {
        client_->getLogger()->appendText("Socket is not in connected state. \n");
        return false;
    }

    return true;
}

TextSender::TextSender(QTcpSocket *socket, Client *client)
    : SenderBase(socket, client) {}

void TextSender::sendToServer(const QString &text, const QString cmd)
{
    send(common::resource::create(cmd, 
                                  common::resource::type::Text, 
                                common::resource::status::Ok, 
                                  text.toUtf8()));
}

AttachmentSender::AttachmentSender(QTcpSocket *socket, Client *client)
    : SenderBase(socket, client) {}

void AttachmentSender::sendToServer(const QString &filePath, const QString cmd)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        send(common::resource::create(cmd, 
                                      common::resource::type::Attachment, 
                                    common::resource::status::Ok, 
                                      file.readAll(), 
                                            fileName));
    }
    else
    {
        client_->getLogger()->appendText("Can't open file!\n");
        return;
    }
}

JsonSender::JsonSender(QTcpSocket *socket, Client *client)
    : SenderBase(socket, client) {}

void JsonSender::sendToServer(const QJsonDocument &json, const QString cmd)
{
    send(common::resource::create(cmd, 
                                  common::resource::type::Json, 
                                common::resource::status::Ok, 
                                  json.toJson()));
}

CommandSender::CommandSender(QTcpSocket *socket, Client *client) 
        : SenderBase(socket, client) {}

void CommandSender::sendToServer(const QString cmd)
{
    send(common::resource::create(cmd, common::resource::type::Command));
}

} // end namespace dcis::client
