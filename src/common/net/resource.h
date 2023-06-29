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
    Attachment,
    Json
};

enum class Command : uint32_t
{
    // server commands
    Publish = 0,

    // client commands
    ShowText,
    ShowImage,
    ClearCycles,
    GenerateGraph,
    UpdateGraph,
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
            {
                return "STRING";
            }

            case resource::ResourceType::Json:
            {
                return "JSON";
            }

        case resource::ResourceType::Attachment:
            {
                return "ATTACHMENT";
            }

            default:
            {
                return "UNKNOWN";
            }
        }
    }

    const QString commandToString() const
    {
        switch(command) {
            case Command::Publish:
                return "Publish";
            case Command::UpdateGraph:
                return "UpdateGraph";
            case Command::ShowImage:
                return "ShowImage";
            case Command::ClearCycles:
                return "ClearCycles";
        case Command::GenerateGraph:
            return "GenerateGraph";
            case Command::ShowText:
                return "ShowText";
            default:
                return "Unknown command";
        }
    }

    int bodySize;
    QString fileName;
    Command command;
    ResourceType resourceType;

    static const int HEADER_SIZE = 128;
};

} // end namespace dcis::common::resource

//Q_DECLARE_METATYPE(dcis::common::resource::Header)

#endif // DCIS_COMMON_RESOURCE_RESOURCE_H_
