#ifndef DCIS_COMMON_GRAPH_GRAPH_H_
#define DCIS_COMMON_GRAPH_GRAPH_H_

// App includes
#include <graph/node.h>
#include <graph/edge.h>

// Qt includes
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

// STL includes
#include <memory>
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

    bool hasDirectedEdge(Node* u, Node* v) const;
    bool setEdge(Node* u, Node* v);
    bool setEdge(const std::string& uName, const std::string& vName);
    const EdgeSetType& getEdges() const;

    bool hasNode(Node* node) const;
    bool hasNode(const std::string& nodeName) const;
    Node* getNode(const std::string& nodeName) const;
    NodeListType getNodes() const;
    bool addNode(const Node& node);
    bool addNode(std::string nodeName);

    static Graph fromJSON(QJsonDocument jsonDoc);
    static QJsonDocument toJSON(const Graph& graph);

private:
    EdgeSetType  edges_;
    NodeSetType  nodes_;
    NodeListType cachedNodes_;

    const bool isDirected_;
};

} // end namespace dcis::common::graph
#endif // DCIS_COMMON_GRAPH_GRAPH_H_
