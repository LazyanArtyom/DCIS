#include <net/client.h>

// App includes
#include <graph/graph.h>
#include <user/userinfo.h>
// STL includes

// QT includes
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QThread>

namespace dcis::client
{

Client::Client(common::utils::ILogger *terminalWidget, QObject *parent)
    : terminalWidget_{terminalWidget}, QObject(parent)
{
}

Client::~Client()
{
    socket_->close();
    socket_->deleteLater();
}

void Client::sendText(const QString &text, const QString cmd)
{
    QByteArray body = text.toUtf8();
    sendToServer(common::resource::create(cmd, common::resource::type::Text, common::resource::status::Ok, body));
}

void Client::sendJson(const QJsonDocument &json, const QString cmd)
{
    QByteArray body = json.toJson();
    sendToServer(common::resource::create(cmd, common::resource::type::Json, common::resource::status::Ok, body));
}

void Client::sendAttachment(const QString &filePath, const QString cmd)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        QByteArray body = file.readAll();
        sendToServer(common::resource::create(cmd, common::resource::type::Attachment, common::resource::status::Ok, body, fileName));
    }
    else
    {
        terminalWidget_->appendText("Can't open file!\n");
        return;
    }
}

void Client::sendCommand(const QString cmd)
{
    sendToServer(common::resource::create(cmd, common::resource::type::Command));
}

void Client::handle(const common::resource::Header &header, const QByteArray &body)
{
    QString msg = "Success: data size is " + QString::number(body.size() + 128) +
                  " bytes, Command: " + header.command_ + " ResourceType: " + header.resourceType_ + "\n";
    terminalWidget_->appendText(msg);

    header_ = header;

    if (header.resourceType_ == common::resource::type::Text)
    {
        handleText(body);
    }
    else if (header.resourceType_ == common::resource::type::Json)
    {
        handleJson(body);
    }
    else if (header.resourceType_ == common::resource::type::Attachment)
    {
        handleAttachment(body);
    }
    else if (header.resourceType_ == common::resource::type::Command)
    {
        handleCommand(header.command_);
    }
    else {
        handleUnknown();
    }
}

void Client::handleUnknown()
{
    terminalWidget_->appendText("Unknown message, doing nothing.\n");
}

void Client::handleAttachment(const QByteArray &data)
{
    if (header_.command_ == common::resource::command::client::ShowImage)
    {
        QImage img = QImage::fromData(data);
        emit sigShowImage(img);
    }
}

void Client::handleText(const QByteArray &data)
{
    if (header_.command_ == common::resource::command::PrintText)
    {
        terminalWidget_->appendText(QString::fromUtf8(data));
    }
}

void Client::handleJson(const QByteArray &data)
{
    if (header_.command_ == common::resource::command::client::UpdateGraph)
    {
        QJsonDocument json = QJsonDocument::fromJson(data);
        emit sigUpdateGraph(json);
    }
}

void Client::handleCommand(const QString cmd)
{
    if (header_.command_ == common::resource::command::server::GetUserInfo)
    {
        common::user::UserInfo userInfo;
        userInfo.name = username_;
        userInfo.password = password_;

        sendJson(common::user::UserInfo::toJson(userInfo), common::resource::command::server::SetUserInfo);
    }
    else if (header_.command_ == common::resource::command::StatusUpdate)
    {
        if (header_.status_ == common::resource::status::UserAccepted)
        {
            terminalWidget_->appendText("Succsesfully connected to server\n");
            emit sigUserAccepted(true);
        }
        else if (header_.status_ == common::resource::status::UserDeclined)
        {
            emit sigUserAccepted(false);
        }
        else if (header_.status_ == common::resource::status::UserAlreadyConnected)
        {
            emit sigUserAccepted(false);
        }
    }
}

bool Client::connectToServer(const QString &ip, const QString &port)
{
    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::onDisconected);

    socket_->connectToHost(ip, port.toInt());

    if (socket_->waitForConnected())
    {
        terminalWidget_->appendText("Succsesfully connected to " + ip + ":" + port + "\n");
        return true;
    }
    else
    {
        terminalWidget_->appendText("Error to connected to " + ip + ":" + port + "\n");
        socket_->close();
        socket_->deleteLater();
        return false;
    }
}

bool Client::sendToServer(const QByteArray &data)
{
    if (!checkServerConnected())
    {
        terminalWidget_->appendText("Server does not respond. Please reconnect!\n");
        return false;
    }

    if (socket_->state() != QAbstractSocket::ConnectedState)
    {
        terminalWidget_->appendText("Socket is not in connected state.\n");
        return false;
    }

    QDataStream socketStream(socket_);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);

    socketStream << data;

    if (socket_->state() == QAbstractSocket::ConnectedState)
    {
        QThread::msleep(100);
        if (socket_->waitForBytesWritten())
        {
            terminalWidget_->appendText("Data sent successfully.\n");
        }
        else
        {
            terminalWidget_->appendText("Failed to send data.\n");
            return false;
        }
        socket_->flush();
    }
    else
    {
        terminalWidget_->appendText("Socket is not in connected state. \n");
        return false;
    }

    return true;
}

bool Client::checkServerConnected() const
{
    if (socket_ == nullptr)
    {
        return false;
    }

    return socket_->state() == QAbstractSocket::ConnectedState ? true : false;
}

QString Client::getUserName() const
{
    return username_;
}

QString Client::getPassword() const
{
    return password_;
}

void Client::setUserName(const QString userName)
{
    username_ = userName;
}

void Client::setPassword(const QString password)
{
    password_ = password;
}

void Client::onReadyRead()
{
    QByteArray buffer;
    QTcpSocket *socket = reinterpret_cast<QTcpSocket *>(sender());

    QDataStream socketStream(socket);
    socketStream.setVersion(common::resource::GLOBAL_DATASTREAM_VERSION);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        terminalWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::GLOBAL_HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        common::resource::Header header;
        ds >> header;
        handle(header, buffer.mid(common::resource::GLOBAL_HEADER_SIZE));
    }
    else
    {
        terminalWidget_->appendText("Reading from server...\n");
    }
}

void Client::onDisconected()
{
    terminalWidget_->appendText("Server disconected.\n");
}

} // end namespace dcis::client
