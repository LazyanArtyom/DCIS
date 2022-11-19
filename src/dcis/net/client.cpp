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

    ////////////////////////////
    QJsonDocument doc = graph::Graph::toJSON(graph::Graph(true));

    resource::Body body(doc);
    resource::Header hed{sizeof(body), resource::Command::Publish, resource::ResourceType::Text};



    QDataStream output(&data_, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_4);

    output << hed << body;
    /////////////////////////////

    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::onDisconected);

    socket_->connectToHost("127.0.0.1", 2323);
    utils::log(utils::LogLevel::INFO, "Client started.");

    socket_->write(data_);
    nextBlockSize_ = 0;
}

Client::~Client()
{
    socket_->close();
    socket_->deleteLater();
    utils::log(utils::LogLevel::INFO, "Client ended.");
}

void Client::onDisconected()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    socket->close();
    socket->deleteLater();
}

void Client::handle(resource::Header header, resource::Body body)
{
    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            handleJson(body.data.toJsonDocument());
            break;
        }

        default:
            handleUnknown();
    }
}

void Client::handleUnknown()
{
    utils::log(utils::LogLevel::INFO, "Unknown message, doing nothing.");
}

void Client::handleString(const QString str)
{
    utils::log(utils::LogLevel::INFO, "String passed: ", str.toStdString());

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Publishing string to others.");
            sendToServer(header_, resource::Body(str));
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }
}

void Client::handleJson(const QJsonDocument json)
{
    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Json passed: ", json.toJson().toStdString());
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }
}

void Client::sendToServer(resource::Header header, resource::Body body)
{
    data_.clear();
    QDataStream output(&data_, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_4);

    output << header << body;
    socket_->write(data_);
}

void Client::onReadyRead()
{
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
        utils::log(utils::LogLevel::ERROR, "Datastream error.");
    }
}

} // end namespace dcis::client
