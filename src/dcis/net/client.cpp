#include <net/client.h>

// App includes
#include <graph/graph.h>
#include <utils/debugstream.h>

// STL includes

// QT includes
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>


namespace dcis::client {

Client::Client(QObject* parent)
    : QObject(parent)
{
}

Client::~Client()
{
    socket_->close();
    socket_->deleteLater();
}

void Client::onDisconected()
{
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Server disconected.");
}

void Client::handle(const resource::Header& header, const QByteArray& body)
{
    QString msg = "Success: data size is " + QString::number(body.size() + 128) + " bytes, Command: " + header.commandToString() + " ResourceType: " + header.typeToString();
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);

    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            handleString(QString(body));
            break;
        }

        case resource::ResourceType::Json:
        {
            handleJson(QJsonDocument::fromJson(body));
            break;
        }

        case resource::ResourceType::Attachment:
        {
            handleAttachment(body);
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
        case resource::Command::ShowImage:
        {
            QImage img = QImage::fromData(data);
            emit sigShowImage(img);
            break;
        }
        default:
            return;
    }
}

void Client::handleString(const QString& str)
{
    /*
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            sendToServer(header_, resource::Body(str));
            break;
        }
        case resource::Command::ShowText:
        {
            emit sigShowText(str);
            break;
        }
        default:
            return;
    }*/
}

void Client::handleJson(const QJsonDocument& json)
{
    switch (header_.command)
    {
        case resource::Command::UpdateGraph:
        {
            emit sigUpdateGraph(json);
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
        msg = "Succsesfully connected to " + ip + ":" + port;
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
        return true;
    }
    else
    {
        msg = "Error to connected to " + ip + ":" + port;
        utils::DebugStream::getInstance().log(utils::LogLevel::Error, msg);

        socket_->close();
        socket_->deleteLater();
        return false;
    }
}

bool Client::sendToServer(const QByteArray& data)
{
    if (socket_ == nullptr)
    {
        return false;
    }

    QDataStream socketStream(socket_);
    socketStream.setVersion(resource::DATASTREAM_VERSION);

    socketStream << data;

    if(socket_->state() == QAbstractSocket::ConnectedState)
    {
        if (socket_->waitForBytesWritten())
        {
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Succsesfully sent to server" + QString::number(data.size()) + " bytes");
            return true;
        }
        else
        {
            utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to send to server");
            return false;
        }
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
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Buffer size: : " + QString::number(buffer.size()) + " bytes");

        QByteArray headerData = buffer.mid(0, resource::Header::HEADER_SIZE);
        QDataStream ds(&headerData, QIODevice::ReadWrite);
        resource::Header header;
        ds >> header;
        handle(header, buffer.mid(resource::Header::HEADER_SIZE));
    }
    else
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Reading from server...");
    }
}

} // end namespace dcis::client
