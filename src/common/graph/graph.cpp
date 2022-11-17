#include <graph/graph.h>

// App includes
#include <utils/debugstream.h>

namespace dcis::common::graph
{

Graph::Graph(bool isDirected) : isDirected_(isDirected)
{
}

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

Graph Graph::fromJSON(QJsonDocument jsonDoc)
{
    QJsonObject json = jsonDoc.object();

    bool isDirected = json.value("directed").toBool();
    Graph graph(isDirected);

    foreach (const QJsonValue& node, json.value("nodes").toArray())
    {
        std::string name = node.toObject().value("name").toString().toStdString();
        double x = node.toObject().value("x").toDouble();
        double y = node.toObject().value("y").toDouble();

        graph.addNode(Node(name, QPointF(x, y)));
    }

    foreach (const QJsonValue& edge, json.value("edges").toArray())
    {
        std::string start = edge.toObject().value("start").toString().toStdString();
        std::string end = edge.toObject().value("end").toString().toStdString();

        graph.setEdge(start, end);
    }

    return graph;
}

QJsonDocument Graph::toJSON(const Graph& graph)
{
    QJsonObject jsonObj;

    //insert single datas first
    jsonObj.insert("directed", graph.isDirected());

    // fill nodes
    QJsonArray nodes;
    for (const auto& node : graph.getNodes())
    {
        QJsonObject jsonNode;
        jsonNode.insert("name", node->getName().c_str());
        jsonNode.insert("x", node->getX());
        jsonNode.insert("y", node->getY());

        nodes.push_back(jsonNode);

    }
    jsonObj.insert("nodes", nodes);

    // fill edges
    QJsonArray edges;
    for (const auto& edge : graph.getEdges())
    {
        QJsonObject jsonEdge;
        jsonEdge.insert("start", edge.first->getName().c_str());
        jsonEdge.insert("end", edge.second->getName().c_str());

        edges.push_back(jsonEdge);
    }

    jsonObj.insert("edges", edges);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    return jsonDoc;
}

bool Graph::isDirected() const
{
    return isDirected_;
}


void Graph::print() const
{
    // debugstream
    QJsonObject jsonObj;

    //insert single datas first
    jsonObj.insert("directed", true);

    // fill nodes
    QJsonArray nodes;
    for (int i = 0; i < 4; ++i)
    {
        QJsonObject jsonNode;
        jsonNode.insert("name", "myname");
        jsonNode.insert("x", i + 5);
        jsonNode.insert("y", i + 10);

        nodes.push_back(jsonNode);

    }
    jsonObj.insert("nodes", nodes);

    // fill edges
    QJsonArray edges;
    for (int i = 0; i < 4; ++i)
    {
        QJsonObject jsonEdge;
        jsonEdge.insert("start", "startNode");
        jsonEdge.insert("end", "endNode");

        edges.push_back(jsonEdge);
    }

    jsonObj.insert("edges", edges);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    Graph graph = fromJSON(jsonDoc);

    jsonDoc = toJSON(graph);

    utils::log(utils::LogLevel::INFO, jsonDoc.toJson().toStdString());
}

} // end namespace dcis::common::graph
