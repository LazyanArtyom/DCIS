#ifndef DCIS_CLIENT_HANDLERS_H_
#define DCIS_CLIENT_HANDLERS_H_

// APP includes
#include <net/resource.h>

// QT includes
#include <QByteArray>
#include <QString>

// STL includes
#include <memory>
#include <functional>
#include <unordered_map>

namespace dcis::client
{

class Client;
class APacketHandler
{
  public:
    using CommandHandlerType = std::function<void(const common::resource::Header &, const QByteArray &)>;
    using CommandHandlerMapType = std::unordered_map<QString, CommandHandlerType>;

    virtual ~APacketHandler() = default;
    virtual void handlePacket(const common::resource::Header &header, const QByteArray &body);

  protected:
    void registerCommand(const QString command, CommandHandlerType handler);
    CommandHandlerMapType commandHandlers_;
};

class TextPacketHandler : public APacketHandler
{
  public:
    explicit TextPacketHandler(Client *client);
};

class JsonPacketHandler : public APacketHandler
{
  public:
    explicit JsonPacketHandler(Client *client);
};

class CommandPacketHandler : public APacketHandler
{
  public:
    explicit CommandPacketHandler(Client *client);
};

class AttachmentPacketHandler : public APacketHandler
{
  public:
    explicit AttachmentPacketHandler(Client *client);
};

class PacketHandlerFactory
{
  public:
    using HandlerCreatorType = std::function<std::unique_ptr<APacketHandler>()>;
    using HandlerMapType = std::unordered_map<QString, HandlerCreatorType>;

    PacketHandlerFactory(Client *client);
    ~PacketHandlerFactory() = default;

    void registerHandler(const QString type, HandlerCreatorType creator);
    std::unique_ptr<APacketHandler> createHandler(const QString type) const;

  private:
    Client *client_;
    HandlerMapType handlers_;
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_HANDLERS_H_
