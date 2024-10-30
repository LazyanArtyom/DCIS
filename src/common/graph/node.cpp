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
#include <graph/node.h>

namespace dcis::common::graph
{

Node::Node(const std::string &name) : name_(name) {}
Node::Node(const std::string &name, QPointF euclidePos) : name_(name), euclidePos_(euclidePos) {}

QPointF Node::getEuclidePos() const
{
    return euclidePos_;
}

void Node::setEuclidePos(QPointF euclidePos)
{
    euclidePos_ = euclidePos;
}

void Node::setNodeType(NodeType nodeType)
{
    nodeType_ = nodeType;
}

Node::NodeType Node::getNodeType()
{
    return nodeType_;
}

void Node::incPosDegree()
{
    degPos_++;
}

void Node::incNegDegree()
{
    degNeg_++;
}

void Node::decPosDegree()
{
    if (degPos_ > 0)
    {
        degPos_--;
    }
}

void Node::decNegDegree()
{
    if (degNeg_ > 0)
    {
        degNeg_--;
    }
}

void Node::incUndirDegree()
{
    deg_++;
}

void Node::decUndirDegree()
{
    if (deg_ > 0)
    {
        deg_--;
    }
}

int Node::getNegDegree() const
{
    return degNeg_;
}

int Node::getPosDegree() const
{
    return degPos_;
}

int Node::getDirDegree() const
{
    return degPos_ + degNeg_;
}

int Node::getUndirDegree() const
{
    return deg_;
}

void Node::setDrone(bool isDrone)
{
    drone_ = isDrone;
}

void Node::setAttacker(bool isAttacker)
{
    attacker_ = isAttacker;
}

void Node::setIp(const std::string &ip)
{
    ip_ = ip;
}

void Node::setPort(const std::string &port)
{
    port_ = port;
}

std::string Node::getIp() const
{
    return ip_;
}

std::string Node::getPort() const
{
    return port_;
}

bool Node::isDrone()
{
    return drone_;
}

bool Node::isAttacker()
{
    return attacker_;
}

std::string Node::getName() const
{
    return name_;
}

double Node::getX() const
{
    return euclidePos_.x();
}

double Node::getY() const
{
    return euclidePos_.y();
}

} // end namespace dcis::common::graph
