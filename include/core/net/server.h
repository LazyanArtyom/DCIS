#ifndef DCIS_SERVER_SERVER_H_
#define DCIS_SERVER_SERVER_H_

// App includes
#include <net/resource.h>
#include <utils/debugstream.h>

// QT includes
#include <QMap>
#include <QImage>
#include <QTcpServer>
#include <QTcpSocket>


namespace dcis::server {
using namespace common;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    using DataType = QByteArray;
    using SocketListType = QMap<qintptr, QTcpSocket*>;

    Server(QObject* parent = nullptr);
    ~Server();

    bool run(const int port);
    void incomingConnection(qintptr socketDescriptor) override;

    bool publish(const QByteArray& data);
    void handle(const resource::Header& header, const QByteArray& body);

    void handleUnknown();
    void handleAttachment(const QByteArray& data);
    void handleString(const QString& str);
    void handleJson(const QJsonDocument& json);

    void updateSockets();

public slots:
    void onReadyRead();
    void onDisconected();

private:
    DataType       data_;
    resource::Header header_;
    SocketListType sockets_;
    qintptr currentSocket_;
    quint16 nextBlockSize_;

    const QString WORKING_DIRECTORY_PATH = "";
    const QString UPLOADED_IMAGES_PATH = WORKING_DIRECTORY_PATH + "/uploadedImages";
};

} // end namespace dcis::server
#endif // DCIS_SERVER_SERVER_H_
