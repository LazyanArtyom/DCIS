#ifndef DCIS_SERVER_SERVER_H_
#define DCIS_SERVER_SERVER_H_

// App includes
#include <graphprocessor/graphprocessor.h>
#include <net/resource.h>
#include <user/userinfo.h>
#include <utils/terminalwidget.h>

// QT includes
#include <QImage>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>

namespace dcis::server
{

class Server : public QTcpServer
{
    Q_OBJECT
  public:
    using DataType = QByteArray;
    using ClientMapType = QMap<common::user::UserInfo, QTcpSocket *>;

    Server(common::utils::TerminalWidget *terminalWidget, QObject *parent = nullptr);
    ~Server();

    void addClient(common::user::UserInfo userInfo);

    bool run(const int port);
    void incomingConnection(qintptr socketDescriptor) override;

    // Senders
    bool publish(const QByteArray &data, qintptr socketDesc);
    bool publishAll(const QByteArray &data, QSet<qintptr> excludeSockets);
    void sendText(const QString &text, const QString cmd, qintptr socketDescriptor = -1);
    void sendJson(const QJsonDocument &json, const QString cmd, qintptr socketDescriptor = -1);
    void sendAttachment(const QString &filePath, const QString cmd, qintptr socketDescriptor = -1);
    void sendCommand(const QString cmd, qintptr socketDescriptor = -1);
    void sendStatusUpdate(const QString status, qintptr socketDescriptor = -1);

    // Handlers
    void handleUnknown();
    void handleText(const QByteArray &data);
    void handleJson(const QByteArray &data);
    void handleAttachment(const QByteArray &data);
    void handleCommand(const QString cmd);
    void handle(const common::resource::Header &header, const QByteArray &body);

  public slots:
    void onReadyRead();
    void onDisconected();

private:
    void updateSockets();

  private:
    DataType data_;
    ClientMapType clientMap_;
    common::resource::Header header_;
    common::utils::TerminalWidget *terminalWidget_ = nullptr;

    qintptr currentSocket_;
    dcis::GraphProcessor::commonGraph *commGraph_ = nullptr;
    dcis::GraphProcessor::GraphProcessor *graphProc_ = nullptr;
    size_t imgW_ = 0;
    size_t imgH_ = 0;

    QString CURRENT_IMAGE_PATH;
    const QString WORKING_DIRECTORY_PATH = "";
    const QString UPLOADED_IMAGES_PATH = "/uploadedImages";
};

} // end namespace dcis::server
#endif // DCIS_SERVER_SERVER_H_
