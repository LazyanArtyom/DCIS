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
#include <net/resource.h>
#include <net/clienthandlers.h>
#include <utils/terminalwidget.h>

// Qt includes
#include <QTcpSocket>

// STL includes
#include <memory>

namespace dcis::client {

class Client : public QObject
{
    Q_OBJECT
  public:
    using HeaderType = common::resource::Header;
    using LoggerPtrType = common::utils::ILogger*;
    using PacketHandlerFactoryPtrType = std::unique_ptr<common::resource::PacketHandlerFactory>;

    Client(LoggerPtrType loggerWidget, QObject *parent = nullptr);
    ~Client();

    void handle(const HeaderType &header, const QByteArray &body);

    bool checkServerConnected() const;
    bool connectToServer(const QString &ip, const QString &port);

    QString getUserName() const;
    QString getPassword() const;
    void setUserName(const QString userName);
    void setPassword(const QString password);

    QTcpSocket *getSocket() const;
    LoggerPtrType getLogger() const;

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
    QTcpSocket *socket_;
    LoggerPtrType loggerWidget_;
    PacketHandlerFactoryPtrType packetHandlerFactory_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_MAINWINDOW_H_
