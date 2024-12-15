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
#ifndef DCIS_CLIENT_SENDERS_H_
#define DCIS_CLIENT_SENDERS_H_

// APP includes
#include <net/resource.h>

// QT includes
#include <QTcpSocket>

// STL includes

namespace dcis::client {

class Client;
class SenderBase
{
  public:
    explicit SenderBase(QTcpSocket *socket, Client *client);
    bool send(const QByteArray &data);

protected:
    Client *client_;
    QTcpSocket *socket_;
};

class TextSender : public SenderBase
{
  public:
    explicit TextSender(QTcpSocket *socket, Client *client);
    void sendToServer(const QString &text, const QString cmd);
};

class AttachmentSender : public SenderBase
{
  public:
    explicit AttachmentSender(QTcpSocket *socket, Client *client);
    void sendToServer(const QString &filePath, const QString cmd);
};

class JsonSender : public SenderBase
{
  public:
    explicit JsonSender(QTcpSocket *socket, Client *client);
    void sendToServer(const QJsonDocument &json, const QString cmd);
};

class CommandSender : public SenderBase
{
  public:
    explicit CommandSender(QTcpSocket *socket, Client *client);
    void sendToServer(const QString cmd);
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_SENDERS_H_
