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
    Edge getEdge(Node* u, Node* v) const;
    Edge getEdge(const std::string& uname, const std::string& vname) const;
    bool hasEdge(Node* u, Node* v) const;
    bool hasEdge(const std::string& uname, const std::string& vname) const;
    bool removeEdge(Node* u, Node* v);
    bool removeEdge(const std::string& uname, const std::string& vname);

    const EdgeSetType& getEdges() const;

    bool hasNode(Node* node) const;
    bool hasNode(const std::string& nodeName) const;
    Node* getNode(const std::string& nodeName) const;
    NodeListType getNodes() const;
    bool addNode(const Node& node);
    bool addNode(std::string nodeName);
    std::string getNextNodeName() const;
    bool setNodeName(Node* node, const std::string& newName);
    bool setNodeName(const std::string& oldName, const std::string& newName);
    bool removeNode(Node* node);
    bool removeNode(const std::string& name);
    bool isolateNode(Node* node);
    bool isolateNode(const std::string& name);


    static Graph* fromJSON(QJsonDocument jsonDoc);
    static QJsonDocument toJSON(Graph* graph);

    bool setNodeType(const std::string &nodeName, const Node::NodeType nodeType);
    bool setDrone(const std::string &nodeName, const bool isDrone);
private:
    EdgeSetType  edges_;
    NodeSetType  nodes_;
    NodeListType cachedNodes_;

    const bool isDirected_;
};

} // end namespace dcis::common::graph
#endif // DCIS_COMMON_GRAPH_GRAPH_H_
