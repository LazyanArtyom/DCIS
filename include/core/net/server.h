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
#include <net/serverhandlers.h>
#include <user/userinfo.h>
#include <utils/imageprovider.h>
#include <utils/terminalwidget.h>
#include <graphprocessor/graphprocessor.h>

// QT includes
#include <QImage>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>

namespace dcis::core
{

class Server : public QTcpServer
{
    Q_OBJECT
  public:
    using LoggerPtrType = common::utils::ILogger*;
    using ClientMapType = QMap<common::user::UserInfo, QTcpSocket *>;
    using ImageProviderPtrType = std::shared_ptr<utils::ImageProvider>;
    using PacketHandlerFactoryPtrType = std::unique_ptr<common::resource::PacketHandlerFactory>;

    Server(common::utils::ILogger *terminalWidget, QObject *parent = nullptr);
    ~Server();

    void addClient(common::user::UserInfo userInfo);

    bool run(const int port);
    void incomingConnection(qintptr socketDescriptor) override;

    // Senders
    bool publishWeb(const QByteArray &data);
    bool publish(const QByteArray &data, qintptr socketDesc);
    bool publishAll(const QByteArray &data, QSet<qintptr> excludeSockets);

    void sendText(const QString &text, const QString cmd, qintptr socketDescriptor = -1);
    void sendJson(const QJsonDocument &json, const QString cmd, qintptr socketDescriptor = -1);
    void sendAttachment(const QString &filePath, const QString cmd, qintptr socketDescriptor = -1);
    void sendCommand(const QString cmd, qintptr socketDescriptor = -1);
    void sendStatusUpdate(const QString status, qintptr socketDescriptor = -1);

    void handle(const common::resource::Header &header, const QByteArray &body);

    int getClientsCount() const;
    qintptr getCurrentSocket() const;
    void setCurrentSocket(qintptr socketDescriptor);
    
    common::utils::ILogger *getTerminalWidget() const;
    std::shared_ptr<utils::ImageProvider> getImageProvider() const;

  public slots:
    void onReadyRead();
    void onDisconected();

private:
    void updateSockets();

  private:
    qintptr currentSocket_;
    ClientMapType clientMap_;
    LoggerPtrType terminalWidget_;
    ImageProviderPtrType imageProvider_;
    PacketHandlerFactoryPtrType packetHandlerFactory_;

    GraphProcessor::commonGraph *commGraph_ = nullptr;
    GraphProcessor::GraphProcessor *graphProc_ = nullptr;
};

} // end namespace dcis::core
#endif // DCIS_CORE_SERVER_H_
