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
//    connect(socket_, &QTcpSocket::disconnected, this, &Server::onDisconected);
    connect(socket_, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);

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

void Client::sendToServer(const QString msg)
{
    /*
    data_.clear();
    QDataStream output(&data_, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_4); 

    output << sizeof(res.header) << res.header << res.body;
    output.device()->seek(0);
    output << quint16(data_.size() - sizeof(quint16));

    socket_->write(data_);*/
}

void Client::onReadyRead()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_4);

    if (input.status() == QDataStream::Ok)
    {
        resource::Header head;
        while (true)
        {
            if (nextBlockSize_ == 0)
            {
                if (socket->bytesAvailable() < (qint64)sizeof(resource::Header))
                {
                    break;
                }
                input >> head;
                nextBlockSize_ = head.bodySize;
            }

            if (socket->bytesAvailable() < nextBlockSize_)
            {
                break;
            }

            resource::Body body;
            input >> body;
            graph::Graph gr = graph::Graph::fromJSON(body.data.toJsonDocument());
            //utils::log(utils::LogLevel::INFO, "Output: \n", str.toStdString());

            nextBlockSize_ = 0;
            //resourceHandler_.handle(head, body);
            break;
        }

    }
    else
    {
        utils::log(utils::LogLevel::ERROR, "Datastream error.");
    }

}

} // end namespace dcis::client
