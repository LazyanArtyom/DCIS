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
#ifndef DCIS_CORE_SERVER_H_
#define DCIS_CORE_SERVER_H_

// App includes
#include <net/resource.h>
#include <user/userinfo.h>
#include <net/serverhandlers.h>
#include <utils/imageprovider.h>
#include <utils/terminalwidget.h>
#include <graphprocessor/graphprocessor.h>

// QT includes
#include <QMap>
#include <QImage>
#include <QTcpServer>
#include <QTcpSocket>

namespace dcis::core {

class Server : public QTcpServer
{
    Q_OBJECT
  public:
    using UserInfoType  = common::user::UserInfo;
    using LoggerPtrType = common::utils::ILogger*;
    using HeaderType    = common::resource::Header;
    using ClientMapType = QMap<common::user::UserInfo, QTcpSocket *>;
    using ImageProviderPtrType = std::shared_ptr<utils::ImageProvider>;
    using PacketHandlerFactoryPtrType = std::unique_ptr<common::resource::PacketHandlerFactory>;
    using CommonGraphType = GraphProcessor::commonGraph;
    using GraphProcType = GraphProcessor::GraphProcessor;

    Server(LoggerPtrType terminalWidget, QObject *parent = nullptr);
    ~Server();

    bool run(const int port);
    void addClient(UserInfoType userInfo);
    void handle(const HeaderType &header, const QByteArray &body);

    qintptr getCurrentSocket() const;
    void setCurrentSocket(qintptr socketDescriptor);

    int getClientsCount() const;
    LoggerPtrType getLogger() const;
    ClientMapType getClientMap() const;
    ImageProviderPtrType getImageProvider() const;

    void setCommGraph(CommonGraphType* commGraph);
    CommonGraphType* getCommGraph() const;
    void setGraphProc(GraphProcType* gp);
    GraphProcType* getGraphProc() const;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

  public slots:
    void onReadyRead();
    void onDisconected();

  private:
    void updateSockets();

  private:
    qintptr currentSocket_;
    ClientMapType clientMap_;
    LoggerPtrType loggerWidget_;
    ImageProviderPtrType imageProvider_;
    PacketHandlerFactoryPtrType packetHandlerFactory_;

    GraphProcessor::commonGraph *commGraph_ = nullptr;
    GraphProcessor::GraphProcessor *graphProc_ = nullptr;
};

} // end namespace dcis::core
#endif // DCIS_CORE_SERVER_H_
