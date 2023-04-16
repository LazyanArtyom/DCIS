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
    /*
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
    }*/
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
    /*
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
    }*/
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

bool Client::getAttachment()
{
    /*
    QByteArray buffer;

    QDataStream socketStream(socket_);
    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket_->socketDescriptor());
        //emit newMessage(message);
        return false;
    }

    QString header = buffer.mid(0,128);
    QString fileType = header.split(",")[0].split(":")[1];

    buffer = buffer.mid(128);

    if(fileType=="attachment"){
        QString fileName = header.split(",")[1].split(":")[1];
        QString ext = fileName.split(".")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

      //  if (QMessageBox::Yes == QMessageBox::question(this, "QTCPServer", QString("You are receiving an attachment from sd:%1 of size: %2 bytes, called %3. Do you want to accept it?").arg(socket->socketDescriptor()).arg(size).arg(fileName)))
        if (true)
        {
            QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/"+fileName, QString("File (*.%1)").arg(ext));

            QFile file(filePath);
            if(file.open(QIODevice::WriteOnly)){
                file.write(buffer);
                QString message = QString("INFO :: Attachment from sd:%1 successfully stored on disk under the path %2").arg(socket->socketDescriptor()).arg(QString(filePath));
                //emit newMessage(message);
            }else {}
                //QMessageBox::critical(this,"QTCPServer", "An error occurred while trying to write the attachment.");
        }else{
            QString message = QString("INFO :: Attachment from sd:%1 discarded").arg(socket->socketDescriptor());
            //emit newMessage(message);
        }
    }else if(fileType=="message"){
        QString message = QString("%1 :: %2").arg(socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString()));
        //emit newMessage(message);
    }*/
return true;
}

bool Client::senAttachment()
{
  if (socket_) {
    if (socket_->isOpen()) {
      QString filePath = QFileDialog::getOpenFileName(
          new QWidget(), ("Select an attachment"),
          QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
          ("File (*.json *.txt *.png *.jpg *.jpeg)"));
/*
      if (filePath.isEmpty()) {
        QMessageBox::critical(this, "QTCPClient",
                              "You haven't selected any attachment!");
        return;
      }*/

      QFile m_file(filePath);
      if (m_file.open(QIODevice::ReadOnly)) {

        QFileInfo fileInfo(m_file.fileName());
        QString fileName(fileInfo.fileName());

        QDataStream socketStream(socket_);
        socketStream.setVersion(QDataStream::Qt_5_15);

        QByteArray header;
        header.prepend(QString("fileType:attachment,fileName:%1,fileSize:%2;")
                           .arg(fileName)
                           .arg(m_file.size())
                           .toUtf8());
        header.resize(128);

        QByteArray byteArray = m_file.readAll();
        byteArray.prepend(header);

        socketStream.setVersion(QDataStream::Qt_5_15);
        socketStream << byteArray;
      } else {}
        //QMessageBox::critical(this, "QTCPClient",
        //                      "Attachment is not readable!");
    } else {}
      //QMessageBox::critical(this, "QTCPClient",
      //                      "Socket doesn't seem to be opened");
  } else {}
    //QMessageBox::critical(this, "QTCPClient", "Not connected");
  return true;
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
