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
#ifndef GRAPHPROCESSOR_H
#define GRAPHPROCESSOR_H

// Qt includes
#include <QFile>
#include <QProcess>
#include <QObject>
// App includes
#include <graphprocessor/node.h>

namespace dcis::core
{
class JsonSender;
}


namespace dcis::GraphProcessor
{

struct DroneInfo
{
    int nDroneType = 0;
    QString ip;
};

class GraphProcessor : public QObject
{
    Q_OBJECT
  public:
    GraphProcessor(QObject* parent = nullptr);
    ~GraphProcessor();
    void setCommGraph(commonGraph *graph);
    size_t getDronesCount();
    Node *getDroneNode(size_t id);
    commonGraph *getCommGraph();
    void initGraph();
    void initGraphDirs();
    void initDrones();
    void clearCycles();
    void generateGraph();
    void startExploration();
    void startAttack();
    void setImgSize(size_t imgW, size_t imgH);
    void startSimulation();
    void setJsonSender(dcis::core::JsonSender *pJsonSender);
    static void sendFileToDrone(const QString& serverIP, QString file);
  private:
    commonGraph *commGraph_ = nullptr;
    NodeVectorType vecDronesStartNodes_;
    NodeVectorType vecAttackerNodes_;
    std::vector<QString> vecExplorationFileNames_;
    std::vector<QString> vecAttackFileNames_;
    NodeListType lstNodes_;
    Node *startNode_ = nullptr;
    CommonNodeToNodeMapType commNodeToNodeMap_;
    size_t neighboursCount_ = 0;
    size_t imgW_ = 0;
    size_t imgH_ = 0;
    std::vector<DroneInfo> drones_;
    dcis::core::JsonSender *pJsonSender_ = nullptr;
};

} // end namespace dcis::GraphProcessor

#endif // GRAPHPROCESSOR_H
