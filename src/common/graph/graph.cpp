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
#include <graph/graph.h>

// App includes

// Qt includes
#include <QFile>

namespace dcis::common::graph
{

Graph::Graph(bool isDirected) : isDirected_(isDirected) {}

Graph::Graph(const Graph &other) : isDirected_(other.isDirected_)
{
    clear();
    for (auto &node : other.getNodes())
    {
        addNode(node->getName());
    }

    for (const auto &[u, v] : other.getEdges())
    {
        setEdge(u->getName(), v->getName());
    }
}

bool Graph::hasDirectedEdge(Node *u, Node *v) const
{
    return edges_.find(std::make_pair(u, v)) != edges_.end();
}

bool Graph::hasNode(const std::string &nodeName) const
{
    return nodes_.find(Node(nodeName)) != nodes_.end();
}

bool Graph::hasNode(Node *node) const
{
    if (node == nullptr)
    {
        return false;
    }

    return nodes_.find(*node) != nodes_.end();
}

Node *Graph::getNode(const std::string &nodeName) const
{
    if (!hasNode(nodeName))
    {
        return nullptr;
    }
    return const_cast<Node *>(&(*nodes_.find(Node(nodeName))));
}

bool Graph::setEdge(Node *u, Node *v)
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

Edge Graph::getEdge(Node *u, Node *v) const
{
    auto it = edges_.find({u, v});
    if (isDirected_)
    {
        return Edge(it);
    }

    it = (it != edges_.end()) ? it : edges_.find({v, u});
    if (it == edges_.end())
    {
        throw "Edge not found";
    }

    return Edge(it);
}

Edge Graph::getEdge(const std::string &uname, const std::string &vname) const
{
    return getEdge(getNode(uname), getNode(vname));
}

bool Graph::hasEdge(Node *u, Node *v) const
{
    return isDirected_ ? hasDirectedEdge(u, v) : (hasDirectedEdge(u, v) || hasDirectedEdge(v, u));
}

bool Graph::hasEdge(const std::string &uname, const std::string &vname) const
{
    return hasEdge(getNode(uname), getNode(vname));
}

bool Graph::removeEdge(Node *u, Node *v)
{
    if (u == v || !hasNode(u) || !hasNode(v))
    {
        return false;
    }

    if (hasDirectedEdge(u, v))
    {
        edges_.erase({u, v});
        if (isDirected_)
        {
            u->decNegDegree();
            v->decPosDegree();
        }
        else
        {
            u->decUndirDegree();
            v->decUndirDegree();
        }

        return true;
    }
    else if (!isDirected_ && hasDirectedEdge(v, u))
    {
        edges_.erase({v, u});
        u->decUndirDegree();
        v->decUndirDegree();
    }

    return false;
}

bool Graph::removeEdge(const std::string &uname, const std::string &vname)
{
    return removeEdge(getNode(uname), getNode(vname));
}

void Graph::setLeftTop(const QString leftTop)
{
    leftTop_ = leftTop;
}

QString Graph::getLeftTop() const
{
    return leftTop_;
}

void Graph::setRightBottom(const QString rightBottom)
{
    rightBottom_ = rightBottom;
}

QString Graph::getRightBottom() const
{
    return rightBottom_;
}

bool Graph::addNode(const Node &node)
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

std::string Graph::getNextNodeName() const
{
    for (size_t i = 0; i < nodes_.size(); ++i)
    {
        auto name = std::string(1, 'a' + (i % 26)) + std::to_string(int(i / 26));
        if (!hasNode(name))
        {
            return name;
        }
    }

    return std::string(1, 'a' + (nodes_.size() % 26)) + std::to_string(int(nodes_.size() / 26));
}

bool Graph::setNodeName(Node *node, const std::string &newName)
{
    if (!hasNode(node) || hasNode(newName) || node->getName() == newName)
    {
        return false;
    }

    std::list<Edge> cachedEdges;
    for (auto it = edges_.begin(); it != edges_.end(); ++it)
    {
        auto edge = Edge(it);
        if (edge.u() == node || edge.v() == node)
        {
            cachedEdges.emplace_back(edge);
        }
    }

    addNode(Node(newName, node->getEuclidePos()));

    for (auto it = edges_.begin(); it != edges_.end(); ++it)
    {
        auto edge = Edge(it);
        if (edge.u()->getName() == node->getName())
        {
            setEdge(newName, edge.v()->getName());
        }
        else if (edge.v()->getName() == node->getName())
        {
            setEdge(edge.u()->getName(), newName);
        }
    }

    removeNode(node);
    return true;
}

bool Graph::setNodeName(const std::string &oldName, const std::string &newName)
{
    return setNodeName(getNode(oldName), newName);
}

bool Graph::removeNode(Node *node)
{
    if (!hasNode(node))
    {
        return false;
    }

    isolateNode(node);
    nodes_.erase(*node);
    cachedNodes_.remove(node);

    return true;
}

bool Graph::removeNode(const std::string &name)
{
    return removeNode(getNode(name));
}

bool Graph::isolateNode(Node *node)
{
    if (!hasNode(node))
    {
        return false;
    }

    std::list<Edge> cachedEdges;
    for (auto it = edges_.begin(); it != edges_.end(); ++it)
    {
        if (Edge(it).u() == node || Edge(it).v() == node)
        {
            cachedEdges.emplace_back(Edge(it));
        }
    }

    for (auto &edge : cachedEdges)
    {
        removeEdge(edge.u(), edge.v());
    }

    return true;
}

bool Graph::isolateNode(const std::string &name)
{
    return isolateNode(getNode(name));
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

const EdgeSetType &Graph::getEdges() const
{
    return edges_;
}

Graph *Graph::fromJSON(QJsonDocument jsonDoc)
{
    QJsonObject json = jsonDoc.object();

    bool isDirected = json.value("directed").toBool();
    Graph *graph = new Graph(isDirected);

    QString leftTop = json.value("leftTop").toString();
    QString rightBottom = json.value("rightBottom").toString();
    graph->setLeftTop(leftTop);
    graph->setRightBottom(rightBottom);

    foreach (const QJsonValue &node, json.value("nodes").toArray())
    {
        std::string name = node.toObject().value("name").toString().toStdString();
        double x = node.toObject().value("x").toDouble();
        double y = node.toObject().value("y").toDouble();
        Node tmpNode(name, QPointF(x, y));
        tmpNode.setType(static_cast<Node::Type>(node.toObject().value("nodeType").toInt()));
        tmpNode.setCategory(static_cast<Node::Category>(node.toObject().value("nodeCategory").toInt()));

        graph->addNode(tmpNode);
    }

    foreach (const QJsonValue &edge, json.value("edges").toArray())
    {
        std::string start = edge.toObject().value("start").toString().toStdString();
        std::string end = edge.toObject().value("end").toString().toStdString();

        graph->setEdge(start, end);
    }

    return graph;
}

QJsonDocument Graph::toJSON(Graph *graph)
{
    QJsonObject jsonObj;

    // insert single datas first
    jsonObj.insert("directed", graph->isDirected());
    jsonObj.insert("leftTop", graph->getLeftTop());
    jsonObj.insert("rightBottom", graph->getRightBottom());

    // fill nodes
    QJsonArray nodes;
    for (const auto &node : graph->getNodes())
    {
        QJsonObject jsonNode;
        jsonNode.insert("name", node->getName().c_str());
        jsonNode.insert("x", node->getX());
        jsonNode.insert("y", node->getY());

        jsonNode.insert("nodeType", (int)(node->getType()));
        jsonNode.insert("nodeCategory", (int)(node->getCategory()));

        nodes.push_back(jsonNode);
    }
    jsonObj.insert("nodes", nodes);

    // fill edges
    QJsonArray edges;
    for (const auto &edge : graph->getEdges())
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

bool Graph::save(Graph *graph, const QString &filePath)
{
    QFile saveFile(filePath);
    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qDebug("Couldn't open save file.");
        return false;
    }

    QJsonDocument jsonDoc = toJSON(graph);
    saveFile.write(jsonDoc.toJson());
    return true;
}

Graph *Graph::load(const QString &filePath)
{
    QFile loadFile(filePath);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return nullptr;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    Graph *graph = fromJSON(loadDoc);
    return graph;
}

bool Graph::isDirected() const
{
    return isDirected_;
}

Graph* Graph::clone() const
{
    Graph* copy = new Graph(this->isDirected_);
    copy->setLeftTop(this->leftTop_);
    copy->setRightBottom(this->rightBottom_);

    // Clone nodes
    for (const auto& node : this->getNodes())
    {
        Node clonedNode(node->getName(), node->getEuclidePos());
        clonedNode.setType(node->getType());
        clonedNode.setCategory(node->getCategory());
        copy->addNode(clonedNode);
    }

    // Clone edges
    for (const auto& [u, v] : this->getEdges())
    {
        if (u && v)
        {
            copy->setEdge(u->getName(), v->getName());
        }
    }

    return copy;
}

Graph* Graph::cloneWithRandomPos() const
{
    Graph* copy = new Graph(this->isDirected_);
    copy->setLeftTop(this->leftTop_);
    copy->setRightBottom(this->rightBottom_);

    // Seed the random generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Clone nodes with random position offset
    for (const auto& node : this->getNodes())
    {
        double offsetX = (std::rand() % 100) - 50; // range: [-50, +49]
        double offsetY = (std::rand() % 100) - 50;

        QPointF newPos = node->getEuclidePos() + QPointF(offsetX, offsetY);

        Node clonedNode(node->getName(), newPos);
        clonedNode.setType(node->getType());
        clonedNode.setCategory(node->getCategory());
        copy->addNode(clonedNode);
    }

    // Clone edges between renamed nodes
    for (const auto& [u, v] : this->getEdges())
    {
        copy->setEdge(u->getName(), v->getName());
    }

    return copy;
}

bool Graph::setNodeType(const std::string &nodeName, const Node::Type nodeType)
{
    auto selectedNode = getNode(nodeName);
    if (selectedNode)
    {
        selectedNode->setType(nodeType);
        return true;
    }
    return false;
}

bool Graph::setNodeCategory(const std::string &nodeName, const Node::Category nodeCategory)
{
    auto selectedNode = getNode(nodeName);
    if (selectedNode)
    {
        selectedNode->setCategory(nodeCategory);
        return true;
    }
    return false;
}

} // end namespace dcis::common::graph
