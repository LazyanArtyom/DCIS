#ifndef DCIS_SERVER_SERVER_H_
#define DCIS_SERVER_SERVER_H_

// App includes
#include <net/resource.h>
#include <utils/debugstream.h>
#include <graphprocessor/graphprocessor.h>

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

    bool publish(const QByteArray& data, bool currentSocket = false);
    void handle(const resource::Header& header, const QByteArray& body);

    void handleUnknown();
    void handleAttachment(const QByteArray& data);
    void handleString(const QByteArray& data);
    void handleJson(const QByteArray& data);

    void syncSockets();
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
    dcis::GraphProcessor::commonGraph* commGraph_ = nullptr;
    dcis::GraphProcessor::GraphProcessor* graphProc_ = nullptr;
    size_t imgW_ = 0;
    size_t imgH_ = 0;

    int masterSocket_ = -1;
    QString CURRENT_IMAGE_PATH;
    const QString WORKING_DIRECTORY_PATH = "";
    const QString UPLOADED_IMAGES_PATH = "/uploadedImages";
};

} // end namespace dcis::server
#endif // DCIS_SERVER_SERVER_H_
