#ifndef DCIS_COMMON_RESOURCE_RESOURCE_H_
#define DCIS_COMMON_RESOURCE_RESOURCE_H_

// App includes
#include <iostream>

// Qt includes
#include <QVariant>
#include <QDataStream>


namespace dcis::common::resource {

// data version to sent packages
const QDataStream::Version DATASTREAM_VERSION = QDataStream::Qt_6_4;

// Message
enum class ResourceType : uint32_t
{
    Text = 0,
    Json,
    Command,
    Attachment,
};

enum class Command : uint32_t
{
    // server commands
    ServerPublish = 0,
    ServerPrintText,
    ServerGetUserInfo,
    ServerUserAccepted,

    // client commands
    ClientShowText,
    ClientShowImage,
    ClientClearCycles,
    ClientStartExploration,
    ClientStartAttack,
    ClientGenerateGraph,
    ClientUpdateGraph,
    ClientSetUserInfo,
};

// Message Header is sent at start of all messages. The template allows us
// to use "enum class" to ensure that the messages are valid at compile time
struct Header
{
    Header() {}
    Header(int size, QString name, Command cmd, ResourceType type)
        : bodySize{size}, fileName{name}, command{cmd}, resourceType{type} {}

    friend QDataStream& operator << (QDataStream& stream, const Header &header)
    {
        stream << header.bodySize;
        stream << header.fileName;
        stream << header.command;
        stream << header.resourceType;

        return stream;
    }

    friend QDataStream& operator >> (QDataStream& stream, Header& header)
    {
        stream >> header.bodySize;
        stream >> header.fileName;
        stream >> header.command;
        stream >> header.resourceType;

        return stream;
    }

    const QString typeToString() const
    {
        switch (resourceType)
        {
            case resource::ResourceType::Text:
                return "STRING";
            case resource::ResourceType::Json:
                return "JSON";
            case ResourceType::Command:
                return "Command";
            case resource::ResourceType::Attachment:
                return "ATTACHMENT";
            default:
                return "UNKNOWN";
        }
    }

    const QString commandToString() const
    {
        switch(command) {

            // Server commands
            case Command::ServerPublish:
                return "ServerPublish";

            case Command::ServerPrintText:
                return "ServerPrintText";

            case Command::ServerGetUserInfo:
                return "ServerGetUserInfo";

            case Command::ServerUserAccepted:
                return "ServerUserAccepted";

            // Client commands
            case Command::ClientUpdateGraph:
                return "ClientUpdateGraph";

            case Command::ClientShowImage:
                return "ClientShowImage";

            case Command::ClientClearCycles:
                return "ClientClearCycles";

            case Command::ClientGenerateGraph:
                return "ClientGenerateGraph";

            case Command::ClientStartExploration:
                return "ClientStartExploration";

            case Command::ClientStartAttack:
                return "ClientStartAttack";

            case Command::ClientShowText:
                return "ClientShowText";

            case Command::ClientSetUserInfo:
                return "ClientSetUserInfo";

            default:
                return "Unknown command";
        }
    }

    int bodySize;
    Command command;
    QString fileName = "";
    ResourceType resourceType;
    static const int HEADER_SIZE = 128;
};

} // end namespace dcis::common::resource
#endif // DCIS_COMMON_RESOURCE_RESOURCE_H_
