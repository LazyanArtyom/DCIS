#ifndef DCIS_COMMON_GRAPH_NODE_H_
#define DCIS_COMMON_GRAPH_NODE_H_

// App includes

// Qt includes
#include <QPointF>

// STL includes
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dcis::common::graph
{

class Node;
using NodeListType = std::list<Node *>;
using NodePairType = std::pair<Node *, Node *>;
using NodeSetType  = std::unordered_set<Node>;
using EdgeSetType  = std::unordered_set<NodePairType>;

class Node
{
  public:
    explicit Node(const std::string &name);
    explicit Node(const std::string &name, QPointF euclidePos);

    enum class NodeType
    {
        Inner,
        Border,
        Corner,
        None
    };

    QPointF getEuclidePos() const;
    void setEuclidePos(QPointF euclidePos);

    void setNodeType(NodeType nodeType);
    NodeType getNodeType();

    void incPosDegree();
    void incNegDegree();
    void decPosDegree();
    void decNegDegree();
    void incUndirDegree();
    void decUndirDegree();

    int getNegDegree() const;
    int getPosDegree() const;
    int getDirDegree() const;
    int getUndirDegree() const;

    bool isDrone();
    bool isAttacker();
    void setDrone(bool isDrone);
    void setAttacker(bool isAttacker);

    void setIp(const std::string &ip);
    void setPort(const std::string &port);

    std::string getIp() const;
    std::string getPort() const;

    double getX() const;
    double getY() const;
    std::string getName() const;

    bool operator==(const Node &other) const
    {
        return (name_ == other.name_);
    }
    bool operator<(const Node &other) const
    {
        return (name_ < other.name_);
    }
    bool operator>(const Node &other) const
    {
        return (name_ > other.name_);
    }

  private:
    int deg_ = 0;
    int degPos_ = 0;
    int degNeg_ = 0;
    bool drone_ = false;
    bool attacker_ = false;
    std::string name_;
    std::string ip_;
    std::string port_;
    QPointF euclidePos_;
    NodeType nodeType_ = NodeType::Inner;
};

} // end namespace dcis::common::graph

template <> struct std::hash<dcis::common::graph::Node>
{
    size_t operator()(const dcis::common::graph::Node &node) const
    {
        return hash<std::string>()(node.getName());
    }
};

#endif // DCIS_COMMON_GRAPH_NODE_H_
