#ifndef DCIS_SERVER_RESOURCEHANDLER_H_
#define DCIS_SERVER_RESOURCEHANDLER_H_

// App includes
#include <net/resource.h>
#include <utils/debugstream.h>

// Qt includes
#include <QString>
#include <QJsonDocument>


namespace dcis::server {
using namespace common;

class Server;
class ResourceHandler
{
public:
    ResourceHandler(Server* server);
    ~ResourceHandler();

    template<typename T>
    void handle(resource::Header header, resource::Body<T> body);

    void handleUnknown();
    void handleString(const QString str);
    void handleJson(const QJsonDocument json);

private:
    resource::Header header_;
    Server* server_ = nullptr;
};

template<typename T>
void ResourceHandler::handle(resource::Header header, resource::Body<T> body)
{
    header_ = header;
    switch (header.resourceType)
    {
        case resource::ResourceType::Text:
        {
            handleJson(body.data);
            break;
        }

        default:
            handleUnknown();
    }
}

} // end namespace dcis::server
#endif // DCIS_SERVER_RESOURCEHANDLER_H_
