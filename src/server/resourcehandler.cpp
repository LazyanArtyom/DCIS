#include <resourcehandler.h>

#include <utils/graphutils.h>

// App includes
#include <server.h>

namespace dcis::server {

ResourceHandler::ResourceHandler(Server* server)
    : server_{server} {}

ResourceHandler::~ResourceHandler()
{

}

void ResourceHandler::handleUnknown()
{
    utils::log(utils::LogLevel::INFO, "Unknown message, doing nothing.");
}

void ResourceHandler::handleString(const QString str)
{
    utils::log(utils::LogLevel::INFO, "String passed: ", str.toStdString());

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Publishing string to others.");
            server_->publish(header_, resource::Body<QString>(str));
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }
}

void ResourceHandler::handleJson(const QJsonDocument json)
{
    utils::log(utils::LogLevel::INFO, "Json passed: ", utils::graph::fromJSON(json).toStdString());

    switch (header_.command)
    {
        case resource::Command::Publish:
        {
            utils::log(utils::LogLevel::INFO, "Publishing JSON to others.");
            server_->publish(header_, resource::Body<QJsonDocument>(json));
            break;
        }

        default:
            utils::log(utils::LogLevel::INFO, "Unknown command, doing nothing.");
    }

}

} // end namespace dcis::server
