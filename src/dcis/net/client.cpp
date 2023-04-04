#include <net/client.h>

// App includes
#include <graph/graph.h>
#include <utils/debugstream.h>

// STL includes
#include <QDebug>


namespace dcis::client {

Client::Client(QObject* parent)
    : QObject(parent)
{
     nextBlockSize_ = 0;
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

void Client::handle(resource::Header header, resource::Body body)
{
    QString msg = "Success: data size is " + QString::number(header.bodySize) + " bytes, Command: " + header_.commandToString() + "DataType: ";

    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            msg += "STRING";
            handleString(body.data.toString());
            break;
        }

        case resource::ResourceType::Json:
        {
            msg += "JSON";
            handleJson(body.data.toJsonDocument());
            break;
        }

        case resource::ResourceType::Image:
        {
            msg += "IMAGE";
            handleImage(body.data.value<QImage>());
            break;
        }

        default:
        {
            msg += "UNKNOWN";
            handleUnknown();
        }
    }

    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
}

void Client::handleUnknown()
{
}

void Client::handleImage(const QImage& img)
{
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            sendToServer(header_, resource::Body(img));
            break;
        }
        case resource::Command::ShowText:
        {
            emit sigShowImage(img);
            break;
        }
        default:
            return;
    }
}

void Client::handleString(const QString& str)
{
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
    }
}

void Client::handleJson(const QJsonDocument& json)
{
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            sendToServer(header_, resource::Body(json));
            break;
        }
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

bool Client::sendToServer(resource::Header header, resource::Body body)
{
    QString msg = "Trying to send to server " + QString::number(header.bodySize) + " bytes, Command: " + header_.commandToString() + "DataType: ";
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);

    data_.clear();
    QDataStream output(&data_, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_4);

    output << header << body;
    socket_->write(data_);

    if(socket_->state() == QAbstractSocket::ConnectedState)
    {
        if (socket_->waitForBytesWritten())
        {
            msg = "Succsesfully sent to server";
            utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg);
            return true;
        }
        else
        {
            msg = "Error to send to server";
            utils::DebugStream::getInstance().log(utils::LogLevel::Error, msg);
            return false;
        }
    }
    else
    {
        msg = "Error to send to server. Connection issue.";
        utils::DebugStream::getInstance().log(utils::LogLevel::Error, msg);
        return false;
    }
}

void Client::onReadyRead()
{
    utils::DebugStream::getInstance().log(utils::LogLevel::Info, "Reading from server ...");

    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_4);

    resource::Header header;
    resource::Body body;

    if (input.status() == QDataStream::Ok)
    {
        while (true)
        {
            if (nextBlockSize_ == 0)
            {
                if (socket->bytesAvailable() < (qint64)sizeof(resource::Header))
                {
                    break;
                }
                input >> header;
                nextBlockSize_ = header.bodySize;
            }

            if (socket->bytesAvailable() < nextBlockSize_)
            {
                break;
            }

            input >> body;
            graph::Graph gr = graph::Graph::fromJSON(body.data.toJsonDocument());

            nextBlockSize_ = 0;
            handle(header, body);
            break;
        }
    }
    else
    {
        utils::DebugStream::getInstance().log(utils::LogLevel::Error, "Error to read, bad package.");
    }
}

} // end namespace dcis::client
