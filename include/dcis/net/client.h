#ifndef DCIS_CLIENT_MAINWINDOW_H_
#define DCIS_CLIENT_MAINWINDOW_H_

// App includes
#include <memory>
#include <net/resource.h>
#include <net/handlers.h>
#include <utils/terminalwidget.h>

// Qt includes
#include <QTcpSocket>

namespace dcis::client
{

class Client : public QObject
{
    Q_OBJECT

  public:
    Client(common::utils::ILogger *terminalWidget, QObject *parent = nullptr);
    ~Client();

    /* Senders */
    void sendText(const QString &text, const QString cmd);
    void sendJson(const QJsonDocument &json, const QString cmd);
    void sendAttachment(const QString &filePath, const QString cmd);
    void sendCommand(const QString cmd);
    bool sendToServer(const QByteArray &data);

    void handle(const common::resource::Header &header, const QByteArray &body);
    
    bool connectToServer(const QString &ip, const QString &port);
    bool checkServerConnected() const;

    QString getUserName() const;
    QString getPassword() const;
    void setUserName(const QString userName);
    void setPassword(const QString password);

    common::utils::ILogger *getTerminalWidget() const;

  public slots:
    void onReadyRead();
    void onDisconected();

  signals:
    void sigUserAccepted(bool isAccepted);
    void sigShowText(const QString &str);
    void sigShowImage(const QImage &img);
    void sigUpdateGraph(const QJsonDocument &json);

  private:
    QString username_;
    QString password_;
    QTcpSocket *socket_ = nullptr;
    common::utils::ILogger *terminalWidget_ = nullptr;
    std::unique_ptr<PacketHandlerFactory> packetHandlerFactory_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_MAINWINDOW_H_
