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
#include <net/serverhandlers.h>

// APP includes
#include <net/server.h>
#include <user/userinfo.h>

// QT includes
#include <QJsonDocument>

namespace dcis::server
{

TextPacketHandler::TextPacketHandler(Server *server)
{
    // command PrintText
    registerCommand(common::resource::command::PrintText,
                    [server](const common::resource::Header &header, const QByteArray &body) {
                    });
}

JsonPacketHandler::JsonPacketHandler(Server *server)
{
    // command Publish
    registerCommand(common::resource::command::server::Publish, [server](const common::resource::Header &header,
                                                                         const QByteArray &body) {
    });

    // command SetUserInfo
    registerCommand(common::resource::command::server::SetUserInfo, [server](const common::resource::Header &header,
                                                                         const QByteArray &body) {
    });
}

CommandPacketHandler::CommandPacketHandler(Server *server)
{
}

AttachmentPacketHandler::AttachmentPacketHandler(Server *server)
{
}

} // end namespace dcis::server
