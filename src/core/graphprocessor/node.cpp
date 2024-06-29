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
