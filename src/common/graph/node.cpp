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

} // end namespace dcis::common::graph
