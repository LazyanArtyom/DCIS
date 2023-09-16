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
            node->setCurrNeighbourId(QRandomGenerator::global()->bounded(node->getNeighbours().size()));
    }
}

void GraphProcessor::clearCycles()
{
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
    }
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Start Node : " + QString::number(startNode_->getID()));
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Curr Neidh id : " + QString::number(startDirId));
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Neigh count : " + QString::number(neighboursCount_));
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "uniqueSteps count : " + QString::number(uniqueSteps.size()));
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Cycles Cleared");
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
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file for drone " + QString::number(i));
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, QString::fromStdString(file.filesystemFileName().string()));
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
        else
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File creation failed");
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
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Creating file for drone " + QString::number(i));
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, QString::fromStdString(file.filesystemFileName().string()));
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
        else
            dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File creation failed");
    }
    dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "Graph Generated");
}

void GraphProcessor::startExploration()
{
    for(size_t i = 0; i < vecExplorationFileNames_.size(); ++i)
    {
        sendFileToDrone(QString::fromStdString(vecDronesStartNodes_[i]->getCommonNode()->getIp()),
                        QString::fromStdString(vecDronesStartNodes_[i]->getCommonNode()->getPort()).toUShort(),
                        vecExplorationFileNames_[i]);
    }
}

void GraphProcessor::startAttack()
{
    for(size_t i = 0; i < vecAttackFileNames_.size(); ++i)
    {
        sendFileToDrone(QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getIp()),
                        QString::fromStdString(vecAttackerNodes_[i]->getCommonNode()->getPort()).toUShort(),
                        vecAttackFileNames_[i]);
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

void GraphProcessor::sendFileToDrone(QString serverIPp, quint16 portp, QFile file)
{

    // Replace these values with your own
    QString username = "user";
    QString serverIP = "proxy55.rt3.io";
    //QString hostname = "proxy50.rt3.io";
    int port = 33890;  // Replace with your desired SSH port

    //QString remoteCommand = "mkdir jujul";

    QString privateKeyPath = "C:\\Users\\Atash\\.ssh\\id_rsa";
    //QString localFilePath = "C:\\Users\\Atash\\Desktop\\build-DCIS-Desktop_Qt_6_5_0_MSVC2019_64bit-Debug\\CMakeCache.txt";
    //QString localFilePath = "/c/Users/Atash/Desktop/build-DCIS-Desktop_Qt_6_5_0_MSVC2019_64bit-Debug/CMakeCache.txt";
    QString localFilePath = file.fileName();
    localFilePath.replace("/","\\");
    QString remoteFilePath = "/home/user";

    // Specify the SCP command and its arguments
    QString scpCommand = "scp";
    QStringList scpArguments;

    // Add the SSH key, port, source, and destination as arguments
    scpArguments << "-P" << QString::number(port) << "-i" << privateKeyPath
                << localFilePath << username + "@" + serverIP + ":" + remoteFilePath;

    // Create a QProcess instance and start the SCP process
    QProcess scpProcess;
    scpProcess.start(scpCommand, scpArguments);
    scpProcess.waitForFinished();
    qDebug() << scpArguments;
    // Check if the process started successfully
    if (scpProcess.state() == QProcess::Running) {
        qDebug() << "SCP transfer in progress...";
    } else {
        qDebug() << "Failed to start the SCP process. Error:" << scpProcess.errorString();
    }

    // Specify the SSH command and its arguments
    QString sshCommand = "ssh";
    QStringList sshArguments;

    // Add the username, hostname, and port as arguments
    sshArguments << "-p" << QString::number(port) << username + "@" + serverIP;

    QString remoteCommand = "python3 /home/user/Desktop/DCIS/droneside/droneprocessor.py > run_dron.log";
    sshArguments << remoteCommand;

    // Create a QProcess instance and start the SSH process
    QProcess sshProcess;
    sshProcess.start(sshCommand, sshArguments);
    sshProcess.waitForStarted();

    if (sshProcess.state() == QProcess::Running) {
            qDebug() << "SSH connection established. Running remote command: " << remoteCommand;

            // Wait for the SSH process to finish
            sshProcess.waitForFinished();

            // Read and print the command output (stdout)
            QByteArray output = sshProcess.readAllStandardOutput();
            qDebug() << "Command output:\n" << output;

            // Read and print any errors (stderr)
            QByteArray error = sshProcess.readAllStandardError();
            qDebug() << "Command error (if any):\n" << error;
        } else {
            qDebug() << "Failed to start the SSH process. Error:" << sshProcess.errorString();
        }
    /*
    if (!file.open(QIODevice::ReadOnly)) {
        // Handle file not found or other errors
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: File not found");
        return;
    }

    QTcpSocket socket;
    socket.connectToHost(serverIP, port);

    if (socket.waitForConnected()) {
        QByteArray fileData = file.readAll();
        socket.write(fileData);
        socket.waitForBytesWritten();
        socket.disconnectFromHost();
        file.close();
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "File sent successfully");
    } else {
        // Handle connection error
        dcis::common::utils::DebugStream::getInstance().log(dcis::common::utils::LogLevel::Info, "ERROR: Failed to connect to the drone");
    }*/

    //QString sshPath = "/home/agit";
    //QString username = "pi";
    //QString password = "raspberry";

    //QString remoteFilePath = "/path/on/remote/server/destination_file";
    //QString localFilePath = "/path/on/local/machine/source_file";
    //QString remoteCommand = "drone.py";

    // SSH connection parameters
    //QStringList sshArgs;
    //sshArgs << "-p" << QString::number(port) << username + "@" + serverIP;

    // File copy parameters
    //QStringList scpArgs;
    //scpArgs << "-P" << QString::number(port) << localFilePath << username + "@" + ipAddress + ":" + remoteFilePath;

    // Remote command execution parameters
    //QStringList sshExecArgs;
    //sshExecArgs << "-p" << QString::number(port) << username + "@" + serverIP << remoteCommand;

    // Start the SSH process for the connection
    //QProcess sshProcess;
    //sshProcess.setProgram(sshPath);
    //sshProcess.setArguments(sshArgs);
    //sshProcess.start();
    //sshProcess.waitForFinished();

    // Copy the file to the remote server using SCP
    //QProcess scpProcess;
    //scpProcess.setProgram("scp");
    //scpProcess.setArguments(scpArgs);
    //scpProcess.start();
    //scpProcess.waitForFinished();

    // Execute the remote command
    //QProcess sshExecProcess;
    //sshExecProcess.setProgram(sshPath);
    //sshExecProcess.setArguments(sshExecArgs);
    //sshExecProcess.start();
    //sshExecProcess.waitForFinished();

    // Check if there were any errors in the processes
    //if (sshProcess.exitCode() != 0 || /*scpProcess.exitCode() != 0 ||*/ sshExecProcess.exitCode() != 0) {
    //    qDebug() << "Error occurred during SSH, SCP, or remote command execution.";
    //    qDebug() << "SSH Process error: " << sshProcess.errorString();
        //qDebug() << "SCP Process error: " << scpProcess.errorString();
    //    qDebug() << "Remote Command Process error: " << sshExecProcess.errorString();
    //} else {
    //    qDebug() << "SSH, SCP, and remote command executed successfully.";
    //}
}

} // end namespace dcis::GraphProcessor
