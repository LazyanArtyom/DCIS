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
#ifndef DCIS_CORE_SENDERS_H_
#define DCIS_CORE_SENDERS_H_

// APP includes
#include <net/resource.h>

// QT includes

// STL includes

namespace dcis::core
{

class Server;
class SenderBase
{
  public:
    explicit SenderBase(const qintptr socketDesc, Server *server);

    bool publish(const qintptr socketDesc, const QByteArray &data);
    bool publishWeb(const QByteArray &data);
    bool publishAll(const QByteArray &data, QSet<qintptr> excludeSockets);

protected:
    Server *server_;
    qintptr socketDescriptor_;
};

class TextSender : public SenderBase
{
  public:
    explicit TextSender(const qintptr socketDesc, Server *server);
    void send(const QString &text, const QString cmd);
};

class AttachmentSender : public SenderBase
{
  public:
    explicit AttachmentSender(const qintptr socketDesc, Server *server);
    void send(const QString &filePath, const QString cmd);
};

class JsonSender : public SenderBase
{
  public:
    explicit JsonSender(const qintptr socketDesc, Server *server);
    void send(const QJsonDocument &json, const QString cmd);
};

class CommandSender : public SenderBase
{
  public:
    explicit CommandSender(const qintptr socketDesc, Server *server);
    void send(const QString cmd);
};

class StatusUpdateSender : public SenderBase
{
  public:
    explicit StatusUpdateSender(const qintptr socketDesc, Server *server);
    void send(const QString status);
};

} // end namespace dcis::core
#endif // DCIS_CORE_SENDERS_H_
