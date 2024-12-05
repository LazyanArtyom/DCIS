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
#ifndef DCIS_CLIENT_MAINWINDOW_H_
#define DCIS_CLIENT_MAINWINDOW_H_

// App includes
#include <memory>
#include <net/resource.h>
#include <net/clienthandlers.h>
#include <utils/terminalwidget.h>

// Qt includes
#include <QTcpSocket>

namespace dcis::client
{

class Client : public QObject
{
    Q_OBJECT

  public:
    using LoggerPtrType = common::utils::ILogger*;
    using PacketHandlerFactoryPtrType = std::unique_ptr<common::resource::PacketHandlerFactory>;

    Client(common::utils::ILogger *terminalWidget, QObject *parent = nullptr);
    ~Client();

    /* Senders */
    void sendText(const QString &text, const QString cmd);
    void sendJson(const QJsonDocument &json, const QString cmd);
    void sendAttachment(const QString &filePath, const QString cmd);
    void sendCommand(const QString cmd);
    bool sendToServer(const QByteArray &data);

    void handle(const common::resource::Header &header, const QByteArray &body);
    
    bool connectToServer(const QString &ip, const QString &port);
    bool checkServerConnected() const;

    QString getUserName() const;
    QString getPassword() const;
    void setUserName(const QString userName);
    void setPassword(const QString password);

    common::utils::ILogger *getTerminalWidget() const;

  public slots:
    void onReadyRead();
    void onDisconected();

  signals:
    void sigUserAccepted(bool isAccepted);
    void sigShowText(const QString &str);
    void sigShowImage(const QImage &img);
    void sigUpdateGraph(const QJsonDocument &json);

  private:
    QString username_;
    QString password_;
    QTcpSocket *socket_ = nullptr;
    LoggerPtrType terminalWidget_ = nullptr;
    PacketHandlerFactoryPtrType packetHandlerFactory_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_MAINWINDOW_H_
