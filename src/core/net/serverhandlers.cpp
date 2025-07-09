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
#include <net/resource.h>
#include <user/userinfo.h>
#include <net/serversenders.h>

// QT includes
#include <QDir>
#include <QThread>
#include <QImageReader>
#include <QJsonDocument>

namespace dcis::core
{

TextPacketHandler::TextPacketHandler(Server *server)
{
    // command PrintText
    registerCommand(common::resource::command::PrintText,
                    [server](const Server::HeaderType &header, const QByteArray &body) {

        server->getLogger()->appendText(QString::fromUtf8(body));
    });
}

JsonPacketHandler::JsonPacketHandler(Server *server)
{
    // command Publish
    registerCommand(common::resource::command::server::Publish, [server](const Server::HeaderType &header,
                                                                         const QByteArray &body) {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(body);

        if (auto commGraph = server->getCommGraph())
        {
            delete commGraph;
        }
        server->setCommGraph(GraphProcessor::commonGraph::fromJSON(jsonDocument));


        if (server->getClientsCount() > 1)
        {
            SenderBase sender(server->getCurrentSocket(), server);
            sender.publishAll(common::resource::create(common::resource::command::client::UpdateGraph, 
                                                       common::resource::type::Json,
                                                       common::resource::status::Ok, 
                                                       body), 
                                                       QSet<qintptr>{ server->getCurrentSocket() }); 
            // send graph to web clients
            sender.publishWeb(body);
        }
    });

    // command SetUserInfo
    registerCommand(common::resource::command::server::SetUserInfo, [server](const Server::HeaderType &header,
                                                                             const QByteArray &body) {

        qintptr currentSocket = server->getCurrentSocket();
        QJsonDocument json = QJsonDocument::fromJson(body);
        Server::UserInfoType userInfo = Server::UserInfoType::fromJson(json);

        // TO DO: Create normal login system
        if (userInfo.name == "artyom" && userInfo.password == "al1234" ||
            userInfo.name == "agit"   && userInfo.password == "aa1234" ||
            userInfo.name == "davit"  && userInfo.password == "dh1234" ||
            userInfo.name == "root"   && userInfo.password == "root")
        {
            server->addClient(userInfo);

            {
                StatusUpdateSender sender(currentSocket, server);
                sender.send(common::resource::status::UserAccepted);
            }

            // sync new user
            if (server->getClientsCount() > 1)
            {
                {
                    AttachmentSender sender(currentSocket, server);
                    sender.send(server->getImageProvider()->getCurrentImagePath(),
                                common::resource::command::client::ShowImage);
                }

                if(auto commGraph = server->getCommGraph())
                {
                    QJsonDocument json = GraphProcessor::commonGraph::toJSON(commGraph);
                    JsonSender sender(currentSocket, server);
                    sender.send(json, common::resource::command::client::UpdateGraph);
                }
            }
        }
        else
        {
            StatusUpdateSender sender(currentSocket, server);
            sender.send(common::resource::status::UserDeclined);
        }
    });
}

CommandPacketHandler::CommandPacketHandler(Server *server)
{
    // command ClearCycles
    registerCommand(common::resource::command::server::ClearCycles, 
                    [server](const Server::HeaderType &header, const QByteArray &body) {


        auto graphProcessor = server->getGraphProc();
        if (graphProcessor)
        {
            delete graphProcessor;
        }
        graphProcessor = new Server::GraphProcType();
        server->setGraphProc(graphProcessor);
        graphProcessor->setCommGraph(server->getCommGraph());
        graphProcessor->initGraph();
        graphProcessor->initGraphDirs();
        graphProcessor->clearCycles();

    });

    // command GenerateGraph
    registerCommand(common::resource::command::server::GenerateGraph, 
                    [server](const Server::HeaderType &header, const QByteArray &body) {

        QSize imgSize = server->getImageProvider()->getCurrentImageSize();
        auto graphProcessor = server->getGraphProc();
        //TODO logging
        //server->getTerminalWidget()->appendText("GenereateGraph\n");

        graphProcessor->setImgSize(imgSize.width(), imgSize.height());
        graphProcessor->generateGraph();
    });

    // command StartExploration
    registerCommand(common::resource::command::server::StartExploration, 
                    [server](const Server::HeaderType &header, const QByteArray &body) {

        auto graphProcessor = server->getGraphProc();
        //TODO logging
        //server->getTerminalWidget()->appendText("StartExploration\n");
        graphProcessor->startExploration();
    });

    // command StartSimulation
    registerCommand(common::resource::command::server::StartSimulation, [server](const Server::HeaderType &header,
                                                                                 const QByteArray &body) {
        
        Q_UNUSED(header);
        Q_UNUSED(body);

        server->startSimulation();
    });

    // command StartAttack
    registerCommand(common::resource::command::server::StartAttack, 
                    [server](const Server::HeaderType &header, const QByteArray &body) {

        // TO DO: start attack
    });
}

AttachmentPacketHandler::AttachmentPacketHandler(Server *server)
{
    registerCommand(common::resource::command::server::Publish,
                    [server](const Server::HeaderType &header, const QByteArray &body) {

        auto imageProvider = server->getImageProvider();
        QString imagePath = imageProvider->getWorkingDirectoryPath() + "/" + header.fileName_;

        imageProvider->saveRawImage(imagePath, body);
        server->getLogger()->appendText("Image saved at " + imagePath);

        SenderBase sender(server->getCurrentSocket(), server);
        sender.publishAll(common::resource::create(common::resource::command::client::ShowImage, 
                                                   common::resource::type::Attachment, 
                                                   common::resource::status::Ok, body),
                                                   QSet<qintptr>{ server->getCurrentSocket() });
    });
}

} // end namespace dcis::core
