#include <graph/graph.h>

// App includes

namespace dcis::common::graph
{

Graph::Graph(bool isDirected) : isDirected_(isDirected) {}

Graph::Graph(const Graph &other)
    : isDirected_(other.isDirected_)
{
    clear();
    for (auto &node: other.getNodes())
    {
        addNode(node->getName());
    }

    for (const auto& [u, v] : other.getEdges())
    {
        setEdge(u->getName(), v->getName());
    }
}

bool Graph::hasDirectedEdge(Node* u, Node* v) const
{
     return edges_.find(std::make_pair(u, v)) != edges_.end();
}

bool Graph::hasNode(const std::string& nodeName) const
{
    return nodes_.find(Node(nodeName)) != nodes_.end();
}

bool Graph::hasNode(Node* node) const
{
    if (node == nullptr)
    {
        return false;
    }

    return nodes_.find(*node) != nodes_.end();
}

Node* Graph::getNode(const std::string &nodeName) const
{
    if (!hasNode(nodeName))
    {
        return nullptr;
    }
    return const_cast<Node *>(&(*nodes_.find(Node(nodeName))));
}

bool Graph::setEdge(Node* u, Node* v)
{
    if (u == v || !hasNode(u) || !hasNode(v))
    {
        return false;
    }
    edges_.insert({u, v});

    return true;
}

bool Graph::setEdge(const std::string &uName, const std::string &vName)
{
    return setEdge(getNode(uName), getNode(vName));
}

bool Graph::addNode(const Node& node)
{
    if (hasNode(node.getName()))
    {
        return false;
    }
    nodes_.insert(node);
    cachedNodes_.emplace_back(getNode(node.getName()));

    return true;
}

bool Graph::addNode(std::string nodeName)
{
    return addNode(Node(nodeName));
}

void Graph::clear()
{
    nodes_.clear();
    edges_.clear();
    cachedNodes_.clear();
}

NodeListType Graph::getNodes() const
{
    return cachedNodes_;
}

const EdgeSetType& Graph::getEdges() const
{
    return edges_;
}

void Graph::print() const
{
    // debugstream
}

} // end namespace dcis::common::graph
