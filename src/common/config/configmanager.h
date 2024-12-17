/*
 * Project Name: DCIS - Drone Collective Intelligence System
 * Copyright (C) 2022 Artyom Lazyan, Agit Atashyan, Davit Hayrapetyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DCIS_COMMON_CONFIG_CONFIGMANAGER_H_
#define DCIS_COMMON_CONFIG_CONFIGMANAGER_H_

// App includes

// Qt includes
#include <QFile>
#include <QString>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtCore/qjsonobject.h>

namespace dcis::common::config {

class ConfigManager
{
  public:
    static QJsonObject loadConfig()
    {
        QFile file("configs.json");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw std::runtime_error("Failed to open config file");
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject())
        {
            throw std::runtime_error("Failed to parse config file");
        }

        return doc.object();
    }

    static QVariant getConfig(const QString &key)
    {
        auto config = loadConfig();
        if (config.contains(key))
        {
            return config[key].toVariant();
        }
        return QVariant();
    }

    static void setConfig(const QString &key, const QVariant &value)
    {
        auto config = loadConfig();
        config[key] = QJsonValue::fromVariant(value);

        QFile file("configs.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            throw std::runtime_error("Failed to open config file");
        }

        QJsonDocument doc(config);
        file.write(doc.toJson());
        file.close();

    }
};

} // namespace dcis::common::config
#endif // DCIS_COMMON_CONFIG_CONFIGMANAGER_H_
