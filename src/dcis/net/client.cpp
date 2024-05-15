#include <net/client.h>

// App includes
#include <graph/graph.h>
#include <user/userinfo.h>
// STL includes

// QT includes
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>


namespace dcis::client {

Client::Client(common::utils::TerminalWidget* terminalWidget, QObject* parent)
    : terminalWidget_{terminalWidget}, QObject(parent)
{
}

Client::~Client()
{
    socket_->close();
    socket_->deleteLater();
}

void Client::sendText(const QString& text, common::resource::Command cmd)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Text;
    header.command = cmd;

    QByteArray resource = text.toUtf8();
    header.bodySize = resource.size();

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);

    resource.prepend(headerData);

    sendToServer(resource);
}

void Client::sendJson(const QJsonDocument& json, common::resource::Command cmd)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Json;
    header.command = cmd;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);

    QByteArray resource = json.toJson();
    header.bodySize = resource.size();

    resource.prepend(headerData);
    sendToServer(resource);
}

void Client::sendAttachment(const QString& filePath, common::resource::Command cmd)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file.fileName());
        QString fileName(fileInfo.fileName());

        common::resource::Header header;
        header.resourceType = common::resource::ResourceType::Attachment;
        header.command = cmd;
        header.fileName = fileName;

        QByteArray headerData;
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        ds << header;

        // header size 128 bytes
        headerData.resize(common::resource::Header::HEADER_SIZE);

        QByteArray resource = file.readAll();

        header.bodySize = resource.size();

        resource.prepend(headerData);
        sendToServer(resource);
    }
    else
    {
        terminalWidget_->appendText("Can't open file!\n");
        return;
    }
}

void Client::sendCommand(const common::resource::Command cmd)
{
    common::resource::Header header;
    header.resourceType = common::resource::ResourceType::Command;
    header.command = cmd;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::Header::HEADER_SIZE);
    sendToServer(headerData);
}

void Client::handle(const common::resource::Header& header, const QByteArray& body)
{
    QString msg = "Success: data size is " + QString::number(body.size() + 128) + " bytes, Command: " + header.commandToString() + " ResourceType: " + header.typeToString() + "\n";
    terminalWidget_->appendText(msg);

    header_ = header;
    switch (header.resourceType)
    {
        case common::resource::ResourceType::Text:
        {
            handleText(body);
            break;
        }

        case common::resource::ResourceType::Json:
        {
            handleJson(body);
            break;
        }

        case common::resource::ResourceType::Attachment:
        {
            handleAttachment(body);
            break;
        }

        case common::resource::ResourceType::Command:
        {
            handleCommand(header.command);
            break;
        }

        default:
        {
            handleUnknown();
        }
    }
}

void Client::handleUnknown()
{
}

void Client::handleAttachment(const QByteArray& data)
{
    switch (header_.command)
    {
        case common::resource::Command::ServerShowImage:
        {
            QImage img = QImage::fromData(data);
            emit sigShowImage(img);
            break;
        }
        default:
            return;
    }
}

void Client::handleText(const QByteArray& data)
{
    switch (header_.command)
    {
    case common::resource::Command::ServerPrintText:
    {
        terminalWidget_->appendText(QString::fromUtf8(data));
        break;
    }
    default:
            return;
    }
}

void Client::handleJson(const QByteArray& data)
{
    switch (header_.command)
    {
        case common::resource::Command::ClientUpdateGraph:
        {
            QJsonDocument json = QJsonDocument::fromJson(data);
            emit sigUpdateGraph(json);
            break;
        }
        default:
            return;
    }
}

void Client::handleCommand(const common::resource::Command cmd)
{
    switch (header_.command)
    {
    case common::resource::Command::ServerGetUserInfo:
    {
        common::user::UserInfo userInfo;
        userInfo.name = username_;
        userInfo.password = password_;

        sendJson(common::user::UserInfo::toJson(userInfo), common::resource::Command::ClientSetUserInfo);
        break;
    }
    case common::resource::Command::ServerUserAccepted:
    {
        QString msg = "Succsesfully connected to server\n";
        terminalWidget_->appendText(msg);

        emit sigUserAccepted(true);
        break;
    }
    case common::resource::Command::ServerUserDeclined:
    {
        emit sigUserAccepted(false);
        break;
    }
    case common::resource::Command::ServerUserAlreadyConnected:
    {
        emit sigUserAccepted(false);
        break;
    }
    default:
            return;
    }
}

bool Client::connectToServer(const QString& ip, const QString& port)
{
    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::onDisconected);

    socket_->connectToHost(ip, port.toInt());

    QString msg;
    if(socket_->waitForConnected())
    {
        msg = "Succsesfully connected to " + ip + ":" + port + "\n";
        terminalWidget_->appendText(msg);
        return true;
    }
    else
    {
        msg = "Error to connected to " + ip + ":" + port + "\n";
        terminalWidget_->appendText(msg);

        socket_->close();
        socket_->deleteLater();
        return false;
    }
}

bool Client::sendToServer(const QByteArray& data)
{
    if (!checkServerConnected())
    {
        terminalWidget_->appendText("Server does not respond. Please reconnect!\n");
        return false;
    }

    if(socket_->state() != QAbstractSocket::ConnectedState)
    {
        terminalWidget_->appendText("Socket is not in connected state.\n");
        return false;
    }

    QDataStream socketStream(socket_);
    socketStream.setVersion(common::resource::DATASTREAM_VERSION);

    socketStream << data;

    if(socket_->state() == QAbstractSocket::ConnectedState)
    {
        QThread::msleep(100);
        if(socket_->waitForBytesWritten())
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
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_6_4);
    socketStream.startTransaction();
    socketStream >> buffer;

    if (socketStream.commitTransaction())
    {
        terminalWidget_->appendText("Buffer size: : " + QString::number(buffer.size()) + " bytes\n");

        QByteArray headerData = buffer.mid(0, common::resource::Header::HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        common::resource::Header header;
        ds >> header;
        handle(header, buffer.mid(common::resource::Header::HEADER_SIZE));
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
