#ifndef GRAPHPROCESSOR_H
#define GRAPHPROCESSOR_H



// App includes
#include <graphprocessor/node.h>


namespace dcis::GraphProcessor {



class GraphProcessor
{
public:
    GraphProcessor();
    void setCommGraph(commonGraph* graph);
    size_t  getDronesCount();
    Node* getDroneNode(size_t id);
    commonGraph* getCommGraph();
    void initGraph();
    void initGraphDirs();
    void clearCycles();
    void generateMap();
    void generateGraph();

private:
    commonGraph* commGraph_ = nullptr;
    NodeVectorType vecDronesStartNodes_;
    NodeListType lstNodes_;
    Node* startNode_ = nullptr;
    CommonNodeToNodeMapType commNodeToNodeMap_;
    size_t neighboursCount_ = 0;
};

#endif // GRAPHPROCESSOR_H

} // end namespace dcis::GraphProcesso
