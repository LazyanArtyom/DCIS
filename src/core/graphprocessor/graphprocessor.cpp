#include <graphprocessor/graphprocessor.h>
#include <utils/debugstream.h>
#include <coordmapper.h>


// Qt includes
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QGeoCoordinate>
#include <QRandomGenerator>
// STL includes
#include <iostream>
namespace dcis::GraphProcessor {

GraphProcessor::GraphProcessor()
{

}

void GraphProcessor::setCommGraph(commonGraph *graph)
{
    if(commGraph_)
        delete commGraph_;
     commGraph_ = graph;
}

size_t GraphProcessor::getDronesCount()
{
    return vecDronesStartNodes_.size();
}

Node *GraphProcessor::getDroneNode(size_t id)
{
    return vecDronesStartNodes_[id];
}

commonGraph* GraphProcessor::getCommGraph()
{
    return commGraph_;
}

void GraphProcessor::initGraph()
{
    CommonNodeListType commNodes = commGraph_->getNodes();
    CommonEdgeSetType commEdges = commGraph_->getEdges();
    size_t id = 0;
    for(auto commNode : commNodes)
    {
        Node* node = new Node(commNode, id);
        lstNodes_.push_back(node);
        commNodeToNodeMap_.insert(std::make_pair(commNode, node));
        id++;
    }
    for(auto node : lstNodes_)
    {
        for(auto edge : commEdges)
        {
            if (edge.first == node -> getCommonNode())
            {
                node -> addNeighbour(commNodeToNodeMap_[edge.second]);
            }
            else if (edge.second == node -> getCommonNode())
            {
                node -> addNeighbour(commNodeToNodeMap_[edge.first]);
            }
        }
        if(startNode_ == nullptr && node->getCommonNode()->getNodeType() == commonNode::NodeType::Corner)
        {
            startNode_ = node;
        }
        if(node -> getCommonNode()->isDrone())
        {
            vecDronesStartNodes_.push_back(node);
        }
    }

    for(auto node : lstNodes_)
        neighboursCount_ += node->getNeighbours().size();

}

void GraphProcessor::initGraphDirs()
{
    Node* nodeIter = startNode_;
    NodeVectorType Neighbours = nodeIter->getNeighbours();
    for(size_t i = 0; i < Neighbours.size(); i++)
    {
        if(Neighbours[i]->getCommonNode()->getNodeType() == commonNode::NodeType::Border ||
                Neighbours[i]->getCommonNode()->getNodeType() == commonNode::NodeType::Corner)
        {
            nodeIter ->setCurrNeighbourId(i);
            break;
        }
    }
    nodeIter = nodeIter->getCurrNeighbour();
    for(;nodeIter != startNode_; nodeIter = nodeIter->getCurrNeighbour())
    {
        for(size_t i = 0; i < Neighbours.size(); i++)
        {
            if((Neighbours[i]->getCommonNode()->getNodeType() == commonNode::NodeType::Border ||
                    Neighbours[i]->getCommonNode()->getNodeType() == commonNode::NodeType::Corner) &&
                    Neighbours[i]->getCurrNeighbourId() == -1)
            {
                nodeIter ->setCurrNeighbourId(i);
                break;
            }
        }
        if(nodeIter->getCurrNeighbourId() == -1)
        {
            for(size_t j = 0; j < nodeIter->getNeighbours().size(); j++)
            {
                if(nodeIter->getNeighbours()[j] == startNode_)
                {
                    nodeIter->setCurrNeighbourId(j);
                    break;
                }
            }
        }
        Q_ASSERT(nodeIter->getCurrNeighbourId() != -1);
    }
    for(auto node : lstNodes_)
    {
        if(node->getCommonNode()->getNodeType() == commonNode::NodeType::Inner)
            node->setCurrNeighbourId(QRandomGenerator::global()->bounded(node->getNeighbours().size()));
    }
}

void GraphProcessor::clearCycles()
{
    auto currPoint = startNode_;
    size_t startDirId = startNode_->getCurrNeighbourId();
    size_t stepsCount = 0;
    while(true)
    {
        currPoint->incrCurrNeighbourId();
        currPoint = currPoint->getCurrNeighbour();
        stepsCount++;
        if(currPoint == startNode_ && currPoint->getCurrNeighbourId() == startDirId && stepsCount >= neighboursCount_)
            break;
    }
}

void GraphProcessor::generateGraph()
{
    CCoordMapper oGeoCoordMapper(commGraph_->getLeftTop(), commGraph_->getRightBottom(), imgW_, imgH_);
    double lat = 0;
    double lon = 0;

    std::vector<QString> vecFileNames(vecDronesStartNodes_.size());
    for(size_t i = 0; i < vecFileNames.size(); ++i)
    {
        QDir dir;
        QString WORKING_DIR = dir.absolutePath();
        vecFileNames[i] = (WORKING_DIR + QDir::separator() + "drone_" + QString::number(i) + ".data");
        QFile file(vecFileNames[i]);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file for drone " + QString::number(i));
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, QString::fromStdString(file.filesystemFileName().string()));
            QTextStream stream(&file);
            //droneID
            stream << i << "\n";
            //dronesCount
            stream << vecDronesStartNodes_.size() << "\n";
            for(size_t j = 0; j < vecDronesStartNodes_.size(); ++j)
            {
                //droneID
                stream << j << "\n" ;
                //startNodeID
                stream << vecDronesStartNodes_[j]->getID() << "\n";
            }
            //nodesCount
            stream << lstNodes_.size() << "\n";
            for(auto node : lstNodes_)
            {
                //nodeID
                stream << node->getID() << "\n";
                oGeoCoordMapper.pixelToWgs(node->getCommonNode()->getEuclidePos(), lat, lon);
                //x
                stream << lon << "\n";
                //y
                stream << lat << "\n";
                //neighboursCount
                stream << node->getNeighbours().size() << "\n";
                for(auto neighbour : node->getNeighbours())
                    //neighbourID
                    stream << neighbour->getID() << "\n";
                //currNeighbourInd
                stream << node->getCurrNeighbourId() << "\n";
            }
            stream.flush();
            file.close();
        }
        else
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File creation failed");
    }
}

void GraphProcessor::generateMap()
{
    NodeVectorType vecDronesCurrPos;
    std::vector<size_t> vecDronesStartDir;
    std::vector<bool> vecCompleted;
    for(auto drone : vecDronesStartNodes_)
    {
        vecDronesCurrPos.push_back(drone);
        vecDronesStartDir.push_back(drone->getCurrNeighbourId());
        vecCompleted.push_back(false);
    }
    auto checkCompleted = [&vecCompleted](){
        bool res = true;
        for(auto droneRes : vecCompleted)
            res = (res && droneRes);
        return res;
    };

    auto isDroneAlowedToFinish = [&](size_t currDroneNum){
        for (int i = 0; i < vecDronesStartNodes_.size(); ++i) {
            if(vecDronesCurrPos[currDroneNum] == vecDronesStartNodes_[i] &&
                    vecDronesCurrPos[currDroneNum]->getCurrNeighbourId() == vecDronesStartDir[i] &&
                    vecCompleted[i] == false)
            {
                vecCompleted[i] = true;
                return true;
            }
        }
        return false;
    };
    while(!checkCompleted())
    {
        for(size_t droneNum = 0; droneNum < vecDronesCurrPos.size(); ++droneNum)
        {
            vecDronesCurrPos[droneNum]->incrCurrNeighbourId();
            vecDronesCurrPos[droneNum] = vecDronesCurrPos[droneNum]->getCurrNeighbour();
            //Agit generate command for send dron to new pos
            //to get coords see vecDronesCurrPos[droneNum]->getCommonNode()->
            if(isDroneAlowedToFinish(droneNum))
            {
                //Agit generate command for landing;
            }
        }
    }
}

void GraphProcessor::setImgSize(size_t imgW, size_t imgH)
{
    imgW_ = imgW;
    imgH_ = imgH;
}

} // end namespace dcis::GraphProcessor
