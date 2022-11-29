#include <graph/node.h>


namespace dcis::common::graph
{

Node::Node(const std::string &name)
    : name_(name)
{

}

Node::Node(const std::string& name, QPointF euclidePos)
    : name_(name), euclidePos_(euclidePos) {}

QPointF Node::getEuclidePos() const
{
    return euclidePos_;
}

void Node::setEuclidePos(QPointF euclidePos)
{
    euclidePos_ = euclidePos;
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
