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
#include <net/clienthandlers.h>

// APP includes
#include <net/client.h>
#include <user/userinfo.h>
#include <net/clientsenders.h>

// QT includes
#include <QJsonDocument>

namespace dcis::client
{

TextPacketHandler::TextPacketHandler(Client *client)
{
    // command PrintText
    registerCommand(common::resource::command::PrintText,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        client->getLogger()->appendText(QString::fromUtf8(body));
                    });
}

JsonPacketHandler::JsonPacketHandler(Client *client)
{
    // command UpdateGraph
    registerCommand(common::resource::command::client::UpdateGraph,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        QJsonDocument json = QJsonDocument::fromJson(body);
                        emit client->sigUpdateGraph(json);
                    });

    registerCommand(common::resource::command::client::SimulateGraph,
                    [client](const common::resource::Header &header, const QByteArray &body) {
                        QJsonDocument json = QJsonDocument::fromJson(body);
                        emit client->sigSimulateGraph(json);
                    });
}

CommandPacketHandler::CommandPacketHandler(Client *client)
{
    // command UpdateGraph
    registerCommand(common::resource::command::server::GetUserInfo,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        common::user::UserInfo userInfo;
                        userInfo.name = client->getUserName();
                        userInfo.password = client->getPassword();

                        // sending user info to server
                        JsonSender sender(client->getSocket(), client);
                        sender.sendToServer(common::user::UserInfo::toJson(userInfo), 
                                             common::resource::command::server::SetUserInfo);
                    });

    // command StatusUpdate
    registerCommand(common::resource::command::StatusUpdate,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        // status UserAccepted
                        if (header.status_ == common::resource::status::UserAccepted)
                        {
                            client->getLogger()->appendText("Succsesfully connected to server\n");
                            emit client->sigUserAccepted(true);
                        }
                        // status UserDeclined
                        else if (header.status_ == common::resource::status::UserDeclined)
                        {
                            emit client->sigUserAccepted(false);
                        }
                        // status UserAlreadyConnected
                        else if (header.status_ == common::resource::status::UserAlreadyConnected)
                        {
                            emit client->sigUserAccepted(false);
                        }
                    });
}

AttachmentPacketHandler::AttachmentPacketHandler(Client *client)
{
    // command ShowImage
    registerCommand(common::resource::command::client::ShowImage,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        QImage img = QImage::fromData(body);
                        emit client->sigShowImage(img);
                    });
}

} // end namespace dcis::client
