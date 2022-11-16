#include <graph/node.h>


namespace dcis::common::graph
{

Node::Node(std::string name) : name_(name) {}

//explicit Node(std::string name, QPointF);

std::string Node::getName() const
{
    return name_;
}

} // end namespace dcis::common::graph
