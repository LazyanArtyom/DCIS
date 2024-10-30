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
#ifndef DCIS_COMMON_RESOURCE_RESOURCE_H_
#define DCIS_COMMON_RESOURCE_RESOURCE_H_

// App includes

// Qt includes
#include <QVariant>
#include <QIODevice>
#include <QDataStream>

namespace dcis::common::resource
{

// fixed header size 128Kb
const int GLOBAL_HEADER_SIZE = 128;
// data version to sent packages
const QDataStream::Version GLOBAL_DATASTREAM_VERSION = QDataStream::Qt_6_4;

// resource types
namespace type
{
    constexpr auto Text       = "Text";
    constexpr auto Json       = "Json";
    constexpr auto Command    = "Command";
    constexpr auto Attachment = "Attachment";
}

// status codes
namespace status
{
    constexpr auto Ok                   = "Ok";
    constexpr auto UserAccepted         = "UserAccepted";
    constexpr auto UserDeclined         = "UserDeclined";
    constexpr auto UserAlreadyConnected = "UserAlreadyConnected";
}

// common commands
namespace command
{
    constexpr auto ShowText             = "ShowText";
    constexpr auto PrintText            = "PrintText";
    constexpr auto StatusUpdate         = "StatusUpdate";
}

// client commands
namespace command::client
{
    constexpr auto ShowImage        = "ShowImage";
    constexpr auto UpdateGraph      = "UpdateGraph";
}

// server commands
namespace command::server 
{
    constexpr auto Publish              = "Publish";
    constexpr auto SetUserInfo          = "SetUserInfo";
    constexpr auto ClearCycles          = "ClearCycles";
    constexpr auto StartAttack          = "StartAttack";
    constexpr auto GetUserInfo          = "GetUserInfo";
    constexpr auto GenerateGraph        = "GenerateGraph";
    constexpr auto StartExploration     = "StartExploration";
}

// Message Header is sent at start of all messages. The template allows us
// to use "enum class" to ensure that the messages are valid at compile time
struct Header
{
    Header() = default;
    Header(const int size, const QString name, const QString cmd, const QString type,
           const QString status = status::Ok);

    friend QDataStream &operator>>(QDataStream &stream, Header &header);
    friend QDataStream &operator<<(QDataStream &stream, const Header &header);

    int bodySize_;
    QString status_;
    QString command_;
    QString fileName_ = "";
    QString resourceType_;
};

class APacketHandler
{
  public:
    using CommandHandlerType = std::function<void(const common::resource::Header &, const QByteArray &)>;
    using CommandHandlerMapType = std::unordered_map<QString, CommandHandlerType>;

    virtual ~APacketHandler() = default;
    virtual void handlePacket(const common::resource::Header &header, const QByteArray &body);

  protected:
    void registerCommand(const QString command, CommandHandlerType handler);
    CommandHandlerMapType commandHandlers_;
};

class PacketHandlerFactory
{
  public:
    using HandlerCreatorType = std::function<std::unique_ptr<common::resource::APacketHandler>()>;
    using HandlerMapType = std::unordered_map<QString, HandlerCreatorType>;

    PacketHandlerFactory();
    ~PacketHandlerFactory() = default;

    void registerHandler(const QString type, HandlerCreatorType creator);
    std::unique_ptr<common::resource::APacketHandler> createHandler(const QString type) const;

  private:
    HandlerMapType handlers_;
};

static QByteArray create(const QString cmd, const QString type, const QString status = status::Ok,
                         QByteArray body = QByteArray(), const QString fileName = "")
{
    Header header;
    header.command_ = cmd;
    header.status_ = status;
    header.fileName_ = fileName;
    header.resourceType_ = type;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::GLOBAL_HEADER_SIZE);
    QByteArray resource = body;
    header.bodySize_ = resource.size();
    resource.prepend(headerData);

    return resource;
}

} // end namespace dcis::common::resource
#endif // DCIS_COMMON_RESOURCE_RESOURCE_H_
