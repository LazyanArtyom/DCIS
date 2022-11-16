#ifndef DCIS_COMMON_RESOURCE_RESOURCE_H_
#define DCIS_COMMON_RESOURCE_RESOURCE_H_

// App includes
#include <iostream>

// Qt includes
#include <QDataStream>


namespace dcis::common::resource {

// Message
enum class ResourceType : uint32_t
{
    Text = 5
};

enum class Command : uint32_t
{
    Publish = 7
};

// Message Header is sent at start of all messages. The template allows us
// to use "enum class" to ensure that the messages are valid at compile time
struct Header
{
    Header() {}
    Header(int size, Command cmd, ResourceType type)
        : bodySize{size}, command{cmd}, resourceType{type} {}

    friend QDataStream& operator << (QDataStream& stream, const Header &header)
    {
        stream << header.bodySize;
        stream << header.command;
        stream << header.resourceType;

        return stream;
    }

    friend QDataStream& operator >> (QDataStream& stream, Header& header)
    {
        stream >> header.bodySize;
        stream >> header.command;
        stream >> header.resourceType;

        return stream;
    }

    int bodySize;
    Command command;
    ResourceType resourceType;
};

template <typename T>
struct Body
{
    Body() {}
    Body(T d) : data{d} {}


    friend QDataStream& operator << (QDataStream& stream, const Body<T> &body)
    {
        stream << body.data;
        return stream;
    }

    friend QDataStream& operator >> (QDataStream& stream, Body<T>& body)
    {
        stream >> body.data;
        return stream;
    }

    T data;
};

} // end namespace dcis::common::resource
#endif // DCIS_COMMON_RESOURCE_RESOURCE_H_
