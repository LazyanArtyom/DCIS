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
