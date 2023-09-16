#ifndef GRAPHPROCESSOR_H
#define GRAPHPROCESSOR_H

// Qt includes
#include <QFile>
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
    void startExploration();
    void startAttack();
    void setImgSize(size_t imgW, size_t imgH);
    void sendFileToDrone(QString serverIP, quint16 port, QFile file);

private:

    commonGraph* commGraph_ = nullptr;
    NodeVectorType vecDronesStartNodes_;
    NodeVectorType vecAttackerNodes_;
    std::vector<QString> vecExplorationFileNames_;
    std::vector<QString> vecAttackFileNames_;
    NodeListType lstNodes_;
    Node* startNode_ = nullptr;
    CommonNodeToNodeMapType commNodeToNodeMap_;
    size_t neighboursCount_ = 0;
    size_t imgW_ = 0;
    size_t imgH_ = 0;
};

#endif // GRAPHPROCESSOR_H

} // end namespace dcis::GraphProcesso
