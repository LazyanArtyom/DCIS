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
#include <coordmapper.h>
#include <graphprocessor/graphprocessor.h>

// Qt includes
#include <QDir>
#include <QFuture>
#include <QFutureSynchronizer>
#include <QGeoCoordinate>
#include <QMessageBox>
#include <QProcess>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QTextStream>
#include <QtConcurrent>
#include <QTimer>

// STL includes
#include <set>
namespace dcis::GraphProcessor
{

GraphProcessor::GraphProcessor(QObject* parent) : QObject(parent)
{
    initDrones();
}

void GraphProcessor::setCommGraph(commonGraph *graph)
{
    if (commGraph_)
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

commonGraph *GraphProcessor::getCommGraph()
{
    return commGraph_;
}

void GraphProcessor::initGraph()
{
    CommonNodeListType commNodes = commGraph_->getNodes();
    CommonEdgeSetType commEdges = commGraph_->getEdges();
    size_t id = 0;
    for (auto commNode : commNodes)
    {
        Node *node = new Node(commNode, id);
        lstNodes_.push_back(node);
        commNodeToNodeMap_.insert(std::make_pair(commNode, node));
        id++;
    }
    for (auto node : lstNodes_)
    {
        for (auto edge : commEdges)
        {
            if (edge.first == node->getCommonNode())
            {
                node->addNeighbour(commNodeToNodeMap_[edge.second]);
            }
            else if (edge.second == node->getCommonNode())
            {
                node->addNeighbour(commNodeToNodeMap_[edge.first]);
            }
        }
        if (startNode_ == nullptr && node->getCommonNode()->getType() == commonNode::Type::Border)
        {
            startNode_ = node;
        }
        if (node->getCommonNode()->getCategory() == commonNode::Category::Drone)
        {
            vecDronesStartNodes_.push_back(node);
        }
        if (node->getCommonNode()->getCategory() == commonNode::Category::Attacker)
        {
            vecAttackerNodes_.push_back(node);
        }
    }

    for (auto node : lstNodes_)
        neighboursCount_ += node->getNeighbours().size();
}

void GraphProcessor::initGraphDirs()
{
    Node *nodeIter = startNode_;
    NodeVectorType Neighbours = nodeIter->getNeighbours();
    for (size_t i = 0; i < Neighbours.size(); i++)
    {
        if (Neighbours[i]->getCommonNode()->getType() == commonNode::Type::Border ||
            Neighbours[i]->getCommonNode()->getType() == commonNode::Type::Corner)
        {
            nodeIter->setCurrNeighbourId(i);
            break;
        }
    }
    nodeIter = nodeIter->getCurrNeighbour();
    for (; nodeIter != startNode_; nodeIter = nodeIter->getCurrNeighbour())
    {
        Neighbours = nodeIter->getNeighbours();
        for (size_t i = 0; i < nodeIter->getNeighbours().size(); i++)
        {
            if ((Neighbours[i]->getCommonNode()->getType() == commonNode::Type::Border ||
                 Neighbours[i]->getCommonNode()->getType() == commonNode::Type::Corner) &&
                Neighbours[i]->getCurrNeighbourId() == -1)
            {
                nodeIter->setCurrNeighbourId(i);
                break;
            }
        }
        if (nodeIter->getCurrNeighbourId() == -1)
        {
            for (size_t j = 0; j < nodeIter->getNeighbours().size(); j++)
            {
                if (nodeIter->getNeighbours()[j] == startNode_)
                {
                    nodeIter->setCurrNeighbourId(j);
                    break;
                }
            }
        }
        Q_ASSERT(nodeIter->getCurrNeighbourId() != -1);
    }
    for (auto node : lstNodes_)
    {
        if (node->getCommonNode()->getType() == commonNode::Type::Inner)
            node->setCurrNeighbourId(QRandomGenerator::global()->bounded((int)node->getNeighbours().size()));
    }
}

void GraphProcessor::initDrones()
{
    QFile file("DroneInfos.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    if (in.atEnd()) {
        qWarning() << "File is empty.";
        return;
    }

    bool ok = false;
    int droneCount = in.readLine().trimmed().toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid drone count.";
        return;
    }

    for (int i = 0; i < droneCount && !in.atEnd(); ++i) {
        QString typeLine = in.readLine().trimmed();
        QString ipLine = in.readLine().trimmed();

        int type = typeLine.toInt(&ok);
        if (!ok || ipLine.isEmpty()) {
            qWarning() << "Invalid data at drone index" << i;
            continue;
        }

        drones_.push_back({type, ipLine});
    }
}

void GraphProcessor::clearCycles()
{
    auto currPoint = startNode_;
    size_t startDirId = startNode_->getCurrNeighbourId();
    std::set<std::pair<Node *, Node *>> uniqueSteps;
    while (true)
    {
        currPoint->incrCurrNeighbourId();
        uniqueSteps.insert(std::make_pair(currPoint, currPoint->getCurrNeighbour()));
        currPoint = currPoint->getCurrNeighbour();
        if (currPoint == startNode_ && currPoint->getCurrNeighbourId() == startDirId &&
            uniqueSteps.size() >= neighboursCount_)
            break;
    }
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Start Node : " +
    // QString::number(startNode_->getID()));
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Curr Neidh id : " +
    // QString::number(startDirId));
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Neigh count : " +
    // QString::number(neighboursCount_));
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "uniqueSteps count : " +
    // QString::number(uniqueSteps.size()));
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Cycles Cleared");
}

void GraphProcessor::generateGraph()
{
    CCoordMapper oGeoCoordMapper(commGraph_->getLeftTop(), commGraph_->getRightBottom(), imgW_, imgH_);
    double lat = 0;
    double lon = 0;
    ///////Exploration
    vecExplorationFileNames_.resize(vecDronesStartNodes_.size());
    for (size_t i = 0; i < vecExplorationFileNames_.size(); ++i)
    {
        QDir dir;
        QString WORKING_DIR = dir.absolutePath();
        vecExplorationFileNames_[i] =
            (WORKING_DIR + QDir::separator() + "exploration_" + QString::number(i) + QDir::separator() + "drone.data");
        dir = QFileInfo(vecExplorationFileNames_[i]).dir();
        if (!dir.exists())
        {
            dir.mkpath(".");
        }
        QFile file(vecExplorationFileNames_[i]);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file
            // for drone " + QString::number(i));
            // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info,
            // QString::fromStdString(file.filesystemFileName().string()));
            QTextStream stream(&file);
            // drone type // 0 for exploration
            stream << 0 << "\n";
            // droneID
            stream << i << "\n";
            // dronesCount
            stream << vecDronesStartNodes_.size() << "\n";
            for (size_t j = 0; j < vecDronesStartNodes_.size(); ++j)
            {
                // droneID
                stream << j << "\n";
                // startNodeID
                stream << vecDronesStartNodes_[j]->getID() << "\n";
                //IP
                if(j < drones_.size())
                    stream << drones_[j].ip << "\n";
                else
                {
                    stream << "0.0.0.0" << "\n";
                    //TODO send message to client
                }
            }
            // nodesCount
            stream << lstNodes_.size() << "\n";
            for (auto node : lstNodes_)
            {
                // nodeID
                stream << node->getID() << "\n";
                oGeoCoordMapper.pixelToWgs(node->getCommonNode()->getEuclidePos(), lat, lon);
                // x
                stream << lon << "\n";
                // y
                stream << lat << "\n";
                // neighboursCount
                stream << node->getNeighbours().size() << "\n";
                for (auto neighbour : node->getNeighbours())
                    // neighbourID
                    stream << neighbour->getID() << "\n";
                // currNeighbourInd
                stream << node->getCurrNeighbourId() << "\n";
            }
            stream.flush();
            file.close();
        }
        // else
        // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File
        // creation failed");
    }

    //////Attack
    vecAttackFileNames_.resize(vecAttackerNodes_.size());
    for (size_t i = 0; i < vecAttackFileNames_.size(); ++i)
    {
        QDir dir;
        QString WORKING_DIR = dir.absolutePath();
        vecAttackFileNames_[i] =
            (WORKING_DIR + QDir::separator() + "attack_" + QString::number(i) + QDir::separator() + "drone.data");
        QFile file(vecAttackFileNames_[i]);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file
            // for drone " + QString::number(i));
            // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info,
            // QString::fromStdString(file.filesystemFileName().string()));
            QTextStream stream(&file);
            // drone type // 1 for attack
            stream << 1 << "\n";
            oGeoCoordMapper.pixelToWgs(vecAttackerNodes_[i]->getCommonNode()->getEuclidePos(), lat, lon);
            // x
            stream << lon << "\n";
            // y
            stream << lat << "\n";
            stream.flush();
            file.close();
        }
        // else
        //     dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File
        //     creation failed");
    }
    // dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Graph Generated");
}

void GraphProcessor::startExploration()
{
    QFutureSynchronizer<void> synchronizer;
    if(drones_.size() < vecExplorationFileNames_.size())
    {
        //TODO send error message to client
        return;
    }
    for (size_t i = 0; i < vecExplorationFileNames_.size(); ++i)
    {

        QString ip = drones_[i].ip;
        QFile file(vecExplorationFileNames_[i]);
        QString localPath = file.fileName();
        QFuture<void> future = QtConcurrent::run([&ip, localPath]() { sendFileToDrone(ip, localPath); });
        synchronizer.addFuture(future);
    }

    synchronizer.waitForFinished();
}

void GraphProcessor::startAttack()
{
    QFutureSynchronizer<void> synchronizer;
    for (size_t i = 0; i < vecAttackFileNames_.size(); ++i)
    {
//    TODO: Assign IP to drones 
//        QString ip = QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getIp());
//        int port = QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getPort()).toUShort();
        QString file(vecAttackFileNames_[i]);
//        QFuture<void> future = QtConcurrent::run([ip, port, file]() { sendFileToDrone(ip, port, file); });
//        synchronizer.addFuture(future);
    }
    synchronizer.waitForFinished();
}

void GraphProcessor::startSimulation()
{
    //TODO calculate step distance
    double stepDist = 10;
    NodeVectorType vecDronesCurrPos;
    std::vector<size_t> vecDronesStartDir;
    std::vector<bool> vecLandedInStartPos;
    std::vector<bool> vecDroneLanded;
    std::vector<std::pair<double, double>> vecDronesCurCoords;
    for (auto drone : vecDronesStartNodes_)
    {
        vecDronesCurrPos.push_back(drone);
        vecDronesStartDir.push_back(drone->getCurrNeighbourId());
        vecLandedInStartPos.push_back(false);
        vecDroneLanded.push_back(false);
        vecDronesCurCoords.push_back(std::make_pair(drone->getCommonNode()->getX()
                                                    , drone->getCommonNode()->getY()));
    }
    auto checkCompleted = [&vecLandedInStartPos]() {
        bool res = true;
        for (const auto& droneRes : vecLandedInStartPos)
            res = (res && droneRes);
        return res;
    };

    auto isDroneAlowedToFinish = [&](size_t currDroneNum) {
        for (int i = 0; i < vecDronesStartNodes_.size(); ++i)
        {
            if (vecDronesCurrPos[currDroneNum] == vecDronesStartNodes_[i] &&
                vecDronesCurrPos[currDroneNum]->getCurrNeighbourId() == vecDronesStartDir[i] &&
                vecLandedInStartPos[i] == false)
            {
                vecLandedInStartPos[i] = true;
                return true;
            }
        }
        return false;
    };

    auto calcDist = [](const double& x1, const double& y1, const double& x2, const double& y2){
        return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    };

    auto pointOnSegmentAtDistance = [](double x1, double y1, double x2, double y2, double d) -> std::pair<double, double> {
        double dx = x2 - x1;
        double dy = y2 - y1;
        double length = std::sqrt(dx * dx + dy * dy);

        if (d < 0 || d > length) {
            throw std::invalid_argument("d must be between 0 and the length of segment AB");
        }

        double ux = dx / length;
        double uy = dy / length;

        return { x1 + d * ux, y1 + d * uy };
    };
    auto processDrones = [&](){
        for (size_t droneNum = 0; droneNum < vecDronesCurrPos.size(); ++droneNum)
        {
            auto distRem = stepDist;
            if(!vecDroneLanded[droneNum])
            {
                while(distRem > 0.0)
                {
                    vecDronesCurrPos[droneNum]->incrCurrNeighbourId();
                    auto posiblePos = vecDronesCurrPos[droneNum]->getCurrNeighbour();
                    auto posComNode = posiblePos->getCommonNode();
                    double curDist = calcDist(vecDronesCurCoords[droneNum].first, vecDronesCurCoords[droneNum].second,
                                              posComNode->getX(), posComNode->getY());
                    if(distRem >= curDist)
                    {
                        distRem -= curDist;
                        vecDronesCurCoords[droneNum].first = posComNode->getX();
                        vecDronesCurCoords[droneNum].second = posComNode->getY();
                        vecDronesCurrPos[droneNum] = posiblePos;
                    }
                    else
                    {
                        vecDronesCurCoords[droneNum] = pointOnSegmentAtDistance(vecDronesCurCoords[droneNum].first, vecDronesCurCoords[droneNum].second,
                                                              posComNode->getX(), posComNode->getY(), distRem);
                        vecDronesCurrPos[droneNum]->decrCurrNeighbourId();
                        break;
                    }

                    if (isDroneAlowedToFinish(droneNum))
                    {
                        vecDroneLanded[droneNum] = true;
                        break;
                    }
                }
            }
        }
    };

    QTimer timer;
    connect(&timer, &QTimer::timeout, this, processDrones);
    timer.start(2000);

    if(checkCompleted())
        timer.stop();

}

void GraphProcessor::setImgSize(size_t imgW, size_t imgH)
{
    imgW_ = imgW;
    imgH_ = imgH;
}

void GraphProcessor::sendFileToDrone(const QString& serverIP, QString localFilePath)
{

    // Replace these values with your own
    QString username = "user";

    QString privateKeyPath = "C:\\Users\\Atash\\.ssh\\id_rsa";
    localFilePath.replace("/", "\\");
    QString remoteFilePath = "/home/user";

    // Specify the SCP command and its arguments
    QString scpCommand = "scp";
    QStringList scpArguments;

    // Add the SSH key, source, and destination as arguments
    scpArguments << "-i" << privateKeyPath << localFilePath
                                                      << username + "@" + serverIP + ":" + remoteFilePath;

    // Create a QProcess instance and start the SCP process
    QProcess scpProcess;
    scpProcess.start(scpCommand, scpArguments);
    scpProcess.waitForFinished();
    qDebug() << scpArguments;
    // Check if the process started successfully
    if (scpProcess.state() == QProcess::Running)
    {
        qDebug() << "SCP transfer in progress...";
    }
    else
    {
        qDebug() << "Failed to start the SCP process. Error:" << scpProcess.errorString();
    }

    // Specify the SSH command and its arguments
    QString sshCommand = "ssh";
    QStringList sshArguments;

    // Add the username, hostname as arguments
    sshArguments << username + "@" + serverIP;

    QString remoteCommand =
        "python3 /home/user/Desktop/DCIS/droneside/droneprocessor.py -data /home/user/drone.data > run_dron.log";
    sshArguments << remoteCommand;

    // Create a QProcess instance and start the SSH process
    QProcess sshProcess;
    sshProcess.start(sshCommand, sshArguments);
    sshProcess.waitForStarted();

    if (sshProcess.state() == QProcess::Running)
    {
        qDebug() << "SSH connection established. Running remote command: " << remoteCommand;

               // Wait for the SSH process to finish
        sshProcess.waitForFinished();

               // Read and print the command output (stdout)
        QByteArray output = sshProcess.readAllStandardOutput();
        qDebug() << "Command output:\n" << output;

               // Read and print any errors (stderr)
        QByteArray error = sshProcess.readAllStandardError();
        qDebug() << "Command error (if any):\n" << error;
    }
    else
    {
        qDebug() << "Failed to start the SSH process. Error:" << sshProcess.errorString();
    }
}

} // end namespace dcis::GraphProcessor
