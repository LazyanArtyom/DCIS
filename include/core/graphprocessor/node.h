#ifndef GRAPH_H
#define GRAPH_H

#include <graph/graph.h>

namespace dcis::GraphProcessor
{

using commonGraph = dcis::common::graph::Graph;
using commonNode = dcis::common::graph::Node;

using CommonNodeListType = std::list<commonNode *>;
using CommonNodePairType = std::pair<commonNode *, commonNode *>;
using CommonNodeSetType = std::unordered_set<commonNode>;
using CommonEdgeSetType = std::unordered_set<CommonNodePairType>;

class Node;
using NodeVectorType = std::vector<Node *>;
using NodeListType = std::list<Node *>;
using CommonNodeToNodeMapType = std::unordered_map<commonNode *, Node *>;

class Node
{
  public:
    explicit Node(commonNode *commNode, size_t id);
    void addNeighbour(Node *node);
    Node *getNeighbourById(size_t id);
    Node *getCurrNeighbour();
    int getCurrNeighbourId();
    void incrCurrNeighbourId();
    void setCurrNeighbourId(size_t id);
    NodeVectorType &getNeighbours();
    commonNode *getCommonNode();
    size_t getID();

  private:
    size_t id_;
    commonNode *commNode_;
    NodeVectorType vecNodeNeighbours_;
    int currNeighbourId_ = -1;
};

#endif // GRAPH_H

} // end namespace dcis::GraphProcessor
