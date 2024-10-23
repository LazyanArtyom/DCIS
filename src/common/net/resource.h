#ifndef DCIS_COMMON_RESOURCE_RESOURCE_H_
#define DCIS_COMMON_RESOURCE_RESOURCE_H_

// App includes

// Qt includes
#include <QVariant>
#include <QIODevice>
#include <QDataStream>

namespace dcis::common::resource
{

// fixed header size 128Kb
const int GLOBAL_HEADER_SIZE = 128;
// data version to sent packages
const QDataStream::Version GLOBAL_DATASTREAM_VERSION = QDataStream::Qt_6_4;

// resource types
namespace type
{
    constexpr auto Text       = "Text";
    constexpr auto Json       = "Json";
    constexpr auto Command    = "Command";
    constexpr auto Attachment = "Attachment";
}

// status codes
namespace status
{
    constexpr auto Ok                   = "Ok";
    constexpr auto UserAccepted         = "UserAccepted";
    constexpr auto UserDeclined         = "UserDeclined";
    constexpr auto UserAlreadyConnected = "UserAlreadyConnected";
}

// common commands
namespace command
{
    constexpr auto ShowText             = "ShowText";
    constexpr auto PrintText            = "PrintText";
    constexpr auto StatusUpdate         = "StatusUpdate";
}

// client commands
namespace command::client
{
    constexpr auto ShowImage        = "ShowImage";
    constexpr auto UpdateGraph      = "UpdateGraph";
}

// server commands
namespace command::server 
{
    constexpr auto Publish              = "Publish";
    constexpr auto SetUserInfo          = "SetUserInfo";
    constexpr auto ClearCycles          = "ClearCycles";
    constexpr auto StartAttack          = "StartAttack";
    constexpr auto GetUserInfo          = "GetUserInfo";
    constexpr auto GenerateGraph        = "GenerateGraph";
    constexpr auto StartExploration     = "StartExploration";
}

// Message Header is sent at start of all messages. The template allows us
// to use "enum class" to ensure that the messages are valid at compile time
struct Header
{
    Header() {}
    Header(int size, const QString name, const QString cmd, const QString type, const QString status = status::Ok)
        : bodySize_{size}, fileName_{name}, command_{cmd}, resourceType_{type}, status_{status} {}

    friend QDataStream &operator<<(QDataStream &stream, const Header &header)
    {
        stream << header.status_;
        stream << header.command_;
        stream << header.bodySize_;
        stream << header.fileName_;
        stream << header.resourceType_;

        return stream;
    }

    friend QDataStream &operator>>(QDataStream &stream, Header &header)
    {
        stream >> header.status_;
        stream >> header.command_;
        stream >> header.bodySize_;
        stream >> header.fileName_;
        stream >> header.resourceType_;

        return stream;
    }

    int bodySize_;
    QString status_;
    QString command_;
    QString fileName_ = "";
    QString resourceType_;
};

static QByteArray create(const QString cmd, 
                         const QString type, 
                         const QString status = status::Ok, 
                         QByteArray body = QByteArray(), 
                         const QString fileName = "")
{
    Header header;
    header.command_      = cmd;
    header.status_       = status;
    header.fileName_     = fileName;
    header.resourceType_ = type;

    QByteArray headerData;
    QDataStream ds(&headerData, QIODevice::ReadWrite);
    ds << header;

    // header size 128 bytes
    headerData.resize(common::resource::GLOBAL_HEADER_SIZE);
    QByteArray resource = body;
    header.bodySize_ = resource.size();
    resource.prepend(headerData);

    return resource;
}

} // end namespace dcis::common::resource
#endif // DCIS_COMMON_RESOURCE_RESOURCE_H_
