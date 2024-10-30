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
#include <graphprocessor/node.h>

namespace dcis::GraphProcessor
{

Node::Node(commonNode *commNode, size_t id) : commNode_(commNode), id_(id)
{
}

void Node::addNeighbour(Node *node)
{
    vecNodeNeighbours_.push_back(node);
}

Node *Node::getNeighbourById(size_t id)
{
    if (id < vecNodeNeighbours_.size())
        return vecNodeNeighbours_[id];
    else
        return nullptr;
}

Node *Node::getCurrNeighbour()
{
    return vecNodeNeighbours_[currNeighbourId_];
}

int Node::getCurrNeighbourId()
{
    return currNeighbourId_;
}

void Node::incrCurrNeighbourId()
{
    currNeighbourId_++;
    if (currNeighbourId_ >= vecNodeNeighbours_.size())
        currNeighbourId_ %= vecNodeNeighbours_.size();
}

void Node::setCurrNeighbourId(size_t id)
{
    currNeighbourId_ = id;
}

NodeVectorType &Node::getNeighbours()
{
    return vecNodeNeighbours_;
}

commonNode *Node::getCommonNode()
{
    return commNode_;
}

size_t Node::getID()
{
    return id_;
}

} // end namespace dcis::GraphProcessor
