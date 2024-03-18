#include <graphprocessor/graphprocessor.h>
#include <utils/debugstream.h>
#include <coordmapper.h>


// Qt includes
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QGeoCoordinate>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QProcess>
#include <QFuture>
#include <QFutureSynchronizer>
#include <QtConcurrent>

// STL includes
#include <iostream>
#include <set>
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
        if(startNode_ == nullptr && node->getCommonNode()->getNodeType() == commonNode::NodeType::Border)
        {
            startNode_ = node;
        }
        if(node -> getCommonNode()->isDrone())
        {
            vecDronesStartNodes_.push_back(node);
        }
        if(node -> getCommonNode()->isAttacker())
        {
            vecAttackerNodes_.push_back(node);
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
        Neighbours = nodeIter->getNeighbours();
        for(size_t i = 0; i < nodeIter->getNeighbours().size(); i++)
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
            node->setCurrNeighbourId(QRandomGenerator::global()->bounded((int)node->getNeighbours().size()));
    }
}

void GraphProcessor::clearCycles()
{
    qDebug()<<" skzbi klir";
    auto currPoint = startNode_;
    size_t startDirId = startNode_->getCurrNeighbourId();
    std::set<std::pair<Node*, Node*>> uniqueSteps;
    while(true)
    {
        currPoint->incrCurrNeighbourId();
        uniqueSteps.insert(std::make_pair(currPoint, currPoint->getCurrNeighbour()));
        currPoint = currPoint->getCurrNeighbour();
        if(currPoint == startNode_ && currPoint->getCurrNeighbourId() == startDirId && uniqueSteps.size() >= neighboursCount_)
            break;
        qDebug()<<"klir";
    }
   // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Start Node : " + QString::number(startNode_->getID()));
   // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Curr Neidh id : " + QString::number(startDirId));
   // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Neigh count : " + QString::number(neighboursCount_));
   // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "uniqueSteps count : " + QString::number(uniqueSteps.size()));
   // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Cycles Cleared");
}

void GraphProcessor::generateGraph()
{
    CCoordMapper oGeoCoordMapper(commGraph_->getLeftTop(), commGraph_->getRightBottom(), imgW_, imgH_);
    double lat = 0;
    double lon = 0;
    qDebug() << "####1111";
    ///////Exploration
    vecExplorationFileNames_.resize(vecDronesStartNodes_.size());
    for(size_t i = 0; i < vecExplorationFileNames_.size(); ++i)
    {
        qDebug() << "####2222";
        QDir dir;
        QString WORKING_DIR = dir.absolutePath();
        vecExplorationFileNames_[i] = (WORKING_DIR + QDir::separator() + "exploration_" + QString::number(i)+ QDir::separator() + "drone.data");
        dir = QFileInfo(vecExplorationFileNames_[i]).dir();
        if (!dir.exists())
        {
            dir.mkpath(".");
        }
        QFile file(vecExplorationFileNames_[i]);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            qDebug() << "####3333";
            //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file for drone " + QString::number(i));
            //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, QString::fromStdString(file.filesystemFileName().string()));
            QTextStream stream(&file);
            //drone type // 0 for exploration
            stream << 0 << "\n";
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
        //else
            //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File creation failed");
    }

    //////Attack
    vecAttackFileNames_.resize(vecAttackerNodes_.size());
    for(size_t i = 0; i < vecAttackFileNames_.size(); ++i)
    {
        QDir dir;
        QString WORKING_DIR = dir.absolutePath();
        vecAttackFileNames_[i] = (WORKING_DIR + QDir::separator() + "attack_" + QString::number(i)+ QDir::separator() + "drone.data");
        QFile file(vecAttackFileNames_[i]);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file for drone " + QString::number(i));
            //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, QString::fromStdString(file.filesystemFileName().string()));
            QTextStream stream(&file);
            //drone type // 1 for attack
            stream << 1 << "\n";
            oGeoCoordMapper.pixelToWgs(vecAttackerNodes_[i]->getCommonNode()->getEuclidePos(), lat, lon);
            //x
            stream << lon << "\n";
            //y
            stream << lat << "\n";
            stream.flush();
            file.close();
        }
        //else
        //    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File creation failed");
    }
    //dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Graph Generated");
}

void GraphProcessor::startExploration()
{
    QFutureSynchronizer<void> synchronizer;
    for (size_t i = 0; i < vecExplorationFileNames_.size(); ++i)
    {
        QString ip   = QString::fromStdString(vecDronesStartNodes_[i]->getCommonNode()->getIp());
        int     port = QString::fromStdString(vecDronesStartNodes_[i]->getCommonNode()->getPort()).toInt();
        QString file(vecExplorationFileNames_[i]);

        // Pass arguments by value (or const reference) if possible
        QFuture<void> future = QtConcurrent::run([ip, port, file]() {
            sendFileToDrone(ip, port, file);
        });
        synchronizer.addFuture(future);
    }

    synchronizer.waitForFinished();
}

void GraphProcessor::startAttack()
{
    QFutureSynchronizer<void> synchronizer;
    for(size_t i = 0; i < vecAttackFileNames_.size(); ++i)
    {
        QString ip   = QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getIp());
        int     port = QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getPort()).toUShort();
        QString file(vecAttackFileNames_[i]);
        QFuture<void> future = QtConcurrent::run([ip, port, file]() {
            sendFileToDrone(ip, port, file);
        });
        synchronizer.addFuture(future);
    }
    synchronizer.waitForFinished();
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
