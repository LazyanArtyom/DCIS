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
#ifndef DCIS_COMMON_USERINFO_H_
#define DCIS_COMMON_USERINFO_H_

#include <QHash>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace dcis::common::user
{

/*
 * Keeps info about user
 */
struct UserInfo
{
    QString name;
    QString password;
    bool isStream = false;
    qintptr socketDescriptor;

    explicit UserInfo(const qintptr desc = -1, const QString nm = "", const QString passwd = "")
        : socketDescriptor{desc}, name{nm}, password{passwd} {}

    static UserInfo fromJson(const QJsonDocument &jsonDoc)
    {
        QJsonObject jsonObj = jsonDoc.object();

        UserInfo userInfo;
        userInfo.name = jsonObj["name"].toString();
        userInfo.password = jsonObj["password"].toString();
        userInfo.socketDescriptor = jsonObj["socketDescriptor"].toInt();

        return userInfo;
    }

    static QJsonDocument toJson(const UserInfo &userInfo)
    {
        QJsonObject jsonObj;

        jsonObj["name"] = userInfo.name;
        jsonObj["password"] = userInfo.password;
        jsonObj["socketDescriptor"] = userInfo.socketDescriptor;

        QJsonDocument jsonDoc(jsonObj);
        return jsonDoc;
    }

    // To keep in map as key
    bool operator<(const UserInfo &other) const
    {
        return socketDescriptor < other.socketDescriptor;
    }

    // To keep in set
    bool operator==(const UserInfo &other) const
    {
        return socketDescriptor == other.socketDescriptor;
    }
};

} // end namespace dcis::common::user
#endif // DCIS_COMMON_USERINFO_H_
