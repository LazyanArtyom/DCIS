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
#include <net/resource.h>


namespace dcis::common::resource
{

Header::Header(const int size, const QString name, const QString cmd, const QString type, const QString status)
    : bodySize_{size}, status_{status}, command_{cmd}, fileName_{name}, resourceType_{type}
{
}

QDataStream &operator<<(QDataStream &stream, const Header &header)
{
    stream << header.status_;
    stream << header.command_;
    stream << header.bodySize_;
    stream << header.fileName_;
    stream << header.resourceType_;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, Header &header)
{
    stream >> header.status_;
    stream >> header.command_;
    stream >> header.bodySize_;
    stream >> header.fileName_;
    stream >> header.resourceType_;

    return stream;
}

void APacketHandler::handlePacket(const common::resource::Header &header, const QByteArray &body)
{
    auto handlerIt = commandHandlers_.find(header.command_);
    if (handlerIt != commandHandlers_.end())
    {
        handlerIt->second(header, body);
    }
    else
    {
        qWarning() << "No command handler found for client command: " << header.command_;
    }
}

void APacketHandler::registerCommand(const QString command, CommandHandlerType handler)
{
    commandHandlers_[command] = std::move(handler);
}

PacketHandlerFactory::PacketHandlerFactory()
{
}

void PacketHandlerFactory::registerHandler(const QString type, HandlerCreatorType creator)
{
    handlers_[type] = std::move(creator);
}

std::unique_ptr<common::resource::APacketHandler> PacketHandlerFactory::createHandler(const QString type) const
{
    auto handlerIt = handlers_.find(type);
    if (handlerIt != handlers_.end())
    {
        return handlerIt->second();
    }

    return nullptr;
}

} // end namespace dcis::common::resource
