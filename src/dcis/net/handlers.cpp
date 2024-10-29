#include <net/handlers.h>

// APP includes
#include <net/client.h>
#include <user/userinfo.h>

// QT includes
#include <QJsonDocument>

namespace dcis::client
{
void APacketHandler::handlePacket(const common::resource::Header &header, const QByteArray &body)
{
    auto handlerIt = commandHandlers_.find(header.command_);
    if (handlerIt != commandHandlers_.end())
    {
        handlerIt->second(header, body);
    }
    else
    {
        qWarning() << "No command handler found for client command: " << header.command_;
    }
}

void APacketHandler::registerCommand(const QString command, CommandHandlerType handler)
{
    commandHandlers_[command] = std::move(handler);
}

PacketHandlerFactory::PacketHandlerFactory(Client *client)
{
    // register handlers
    registerHandler(common::resource::type::Text, [client]() { return std::make_unique<TextPacketHandler>(client); });
    registerHandler(common::resource::type::Json, [client]() { return std::make_unique<JsonPacketHandler>(client); });
    registerHandler(common::resource::type::Command,
                    [client]() { return std::make_unique<CommandPacketHandler>(client); });
    registerHandler(common::resource::type::Attachment,
                    [client]() { return std::make_unique<AttachmentPacketHandler>(client); });
}
void PacketHandlerFactory::registerHandler(const QString type, HandlerCreatorType creator)
{
    handlers_[type] = std::move(creator);
}

std::unique_ptr<APacketHandler> PacketHandlerFactory::createHandler(const QString type) const
{
    auto handlerIt = handlers_.find(type);
    if (handlerIt != handlers_.end())
    {
        return handlerIt->second();
    }

    return nullptr;
}

TextPacketHandler::TextPacketHandler(Client *client)
{
    // command PrintText
    registerCommand(common::resource::command::PrintText,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        client->getTerminalWidget()->appendText(QString::fromUtf8(body));
                    });
}

JsonPacketHandler::JsonPacketHandler(Client *client)
{
    // command UpdateGraph
    registerCommand(common::resource::command::client::UpdateGraph,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        QJsonDocument json = QJsonDocument::fromJson(body);
                        emit client->sigUpdateGraph(json);
                    });
}

CommandPacketHandler::CommandPacketHandler(Client *client)
{
    // command UpdateGraph
    registerCommand(common::resource::command::server::GetUserInfo,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        common::user::UserInfo userInfo;
                        userInfo.name = client->getUserName();
                        userInfo.password = client->getPassword();

                        // sending user info to server
                        QByteArray data = common::user::UserInfo::toJson(userInfo).toJson();
                        client->sendToServer(common::resource::create(common::resource::command::server::SetUserInfo,
                                                                       common::resource::type::Json,
                                                                       common::resource::status::Ok, data));
                    });

    // command StatusUpdate
    registerCommand(common::resource::command::StatusUpdate,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        // status UserAccepted
                        if (header.status_ == common::resource::status::UserAccepted)
                        {
                            client->getTerminalWidget()->appendText("Succsesfully connected to server\n");
                            emit client->sigUserAccepted(true);
                        }
                        // status UserDeclined
                        else if (header.status_ == common::resource::status::UserDeclined)
                        {
                            emit client->sigUserAccepted(false);
                        }
                        // status UserAlreadyConnected
                        else if (header.status_ == common::resource::status::UserAlreadyConnected)
                        {
                            emit client->sigUserAccepted(false);
                        }
                    });
}

AttachmentPacketHandler::AttachmentPacketHandler(Client *client)
{
    // command ShowImage
    registerCommand(common::resource::command::client::ShowImage,
                    [client](const common::resource::Header &header, const QByteArray &body) {

                        QImage img = QImage::fromData(body);
                        emit client->sigShowImage(img);
                    });
}

} // end namespace dcis::client
