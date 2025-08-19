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

Node::Node(const Node &node)
{
    deg_ = node.deg_;
    degPos_ = node.degPos_;
    degNeg_ = node.degNeg_;
    drone_ = node.drone_;
    name_ = node.name_;
    euclidePos_ = node.euclidePos_;
    nodeType_ = node.nodeType_;
    nodeCategory_ = node.nodeCategory_;
}

QPointF Node::getEuclidePos() const
{
    return euclidePos_;
}

void Node::setEuclidePos(QPointF euclidePos)
{
    euclidePos_ = euclidePos;
}

void Node::setType(Type nodeType)
{
    nodeType_ = nodeType;
}

Node::Type Node::getType() const
{
    return nodeType_;
}

void Node::setCategory(Category nodeCategory)
{
    nodeCategory_ = nodeCategory;
}

Node::Category Node::getCategory() const
{
    return nodeCategory_;
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

void Node::setX(double x)
{
    euclidePos_.setX(x);
}

void Node::setY(double y)
{
    euclidePos_.setY(y);
}

} // end namespace dcis::common::graph
