#ifndef DCIS_COMMON_GRAPH_NODE_H_
#define DCIS_COMMON_GRAPH_NODE_H_

// App includes

//Qt includes
 #include <QPointF>

// STL includes
#include <list>
#include <string>
#include <unordered_set>
#include <unordered_map>


namespace dcis::common::graph
{

class Node;
using NodeListType = std::list<Node*>;
using NodePairType = std::pair<Node*, Node*>;
using NodeSetType  = std::unordered_set<Node>;
using EdgeSetType  = std::unordered_set<NodePairType>;

class Node
{
public:
    explicit Node(const std::string& name);
    explicit Node(const std::string& name, QPointF euclidePos);

    std::string getName() const;
    double getX() const;
    double getY() const;

    bool operator==(const Node& other) const { return (name_ == other.name_); }
    bool operator<(const Node&  other) const { return (name_ < other.name_);  }
    bool operator>(const Node&  other) const { return (name_ > other.name_);  }

private:
    int deg_    = 0;
    int degPos_ = 0;
    int degNeg_ = 0;
    std::string name_;
    QPointF euclidePos_;
};

} // end namespace dcis::common::graph

template<>
struct std::hash<dcis::common::graph::Node>
{
    size_t operator()(const dcis::common::graph::Node& node) const
    {
        return hash<std::string>()(node.getName());
    }
};

#endif // DCIS_COMMON_GRAPH_NODE_H_
