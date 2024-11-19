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
#ifndef DCIS_COMMON_GRAPH_GRAPH_H_
#define DCIS_COMMON_GRAPH_GRAPH_H_

// App includes
#include <graph/edge.h>
#include <graph/node.h>

// Qt includes
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

// STL includes
#include <string>

namespace dcis::common::graph
{

class Graph
{
  public:
    explicit Graph(bool isDirected);
    Graph(const Graph &other);

    void clear();
    bool isDirected() const;

    bool setEdge(Node *u, Node *v);
    Edge getEdge(Node *u, Node *v) const;
    bool setEdge(const std::string &uName, const std::string &vName);
    Edge getEdge(const std::string &uname, const std::string &vname) const;
    
    bool hasEdge(Node *u, Node *v) const;
    bool hasEdge(const std::string &uname, const std::string &vname) const;
    bool hasDirectedEdge(Node *u, Node *v) const;
    bool removeEdge(Node *u, Node *v);
    bool removeEdge(const std::string &uname, const std::string &vname);

    void setLeftTop(const QString leftTop);
    QString getLeftTop() const;

    void setRightBottom(const QString rightBottom);
    QString getRightBottom() const;

    const EdgeSetType &getEdges() const;

    bool hasNode(Node *node) const;
    bool hasNode(const std::string &nodeName) const;
    Node *getNode(const std::string &nodeName) const;
    NodeListType getNodes() const;
    bool addNode(const Node &node);
    bool addNode(std::string nodeName);
    std::string getNextNodeName() const;
    bool setNodeName(Node *node, const std::string &newName);
    bool setNodeName(const std::string &oldName, const std::string &newName);
    bool removeNode(Node *node);
    bool removeNode(const std::string &name);
    bool isolateNode(Node *node);
    bool isolateNode(const std::string &name);

    static Graph *fromJSON(QJsonDocument jsonDoc);
    static QJsonDocument toJSON(Graph *graph);
    static bool save(Graph *graph, const QString &filePath);
    static Graph *load(const QString &filePath);

    bool setNodeType(const std::string &nodeName, const Node::Type nodeType);
    bool setNodeCategory(const std::string &nodeName, const Node::Category nodeCategory);

  private:
    QString leftTop_;
    QString rightBottom_;
    EdgeSetType edges_;
    NodeSetType nodes_;
    NodeListType cachedNodes_;

    const bool isDirected_ = false;
};

} // end namespace dcis::common::graph
#endif // DCIS_COMMON_GRAPH_GRAPH_H_
