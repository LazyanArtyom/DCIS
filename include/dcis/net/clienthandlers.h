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
#ifndef DCIS_CLIENT_HANDLERS_H_
#define DCIS_CLIENT_HANDLERS_H_

// APP includes
#include <net/resource.h>

// QT includes

// STL includes

namespace dcis::client
{

class Client;
class TextPacketHandler : public common::resource::APacketHandler
{
  public:
    explicit TextPacketHandler(Client *client);
};

class JsonPacketHandler : public common::resource::APacketHandler
{
  public:
    explicit JsonPacketHandler(Client *client);
};

class CommandPacketHandler : public common::resource::APacketHandler
{
  public:
    explicit CommandPacketHandler(Client *client);
};

class AttachmentPacketHandler : public common::resource::APacketHandler
{
  public:
    explicit AttachmentPacketHandler(Client *client);
};

} // end namespace dcis::client
#endif // DCIS_CLIENT_HANDLERS_H_
