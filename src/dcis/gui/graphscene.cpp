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
#include <gui/graphscene.h>

namespace dcis::gui
{

GraphScene::GraphScene(graph::Graph *graph)
{
    setGraph(graph);
}

GraphScene::~GraphScene()
{
    clearAll();
}

graph::Graph *GraphScene::getGraph() const
{
    return graph_;
}

void GraphScene::setGraph(graph::Graph *graph)
{
    if (graph_ != nullptr)
    {
        delete graph_;
    }

    graph_ = graph;
    onReload();
}

NodeItem *GraphScene::getNodeItem(const std::string &name)
{
    if (nodeItems_.find(name) != nodeItems_.end())
    {
        return nodeItems_[name];
    }

    return nullptr;
}

EdgeItem *GraphScene::getEdgeItem(const std::string &uname, const std::string &vname)
{
    if (edgeItems_.find(std::make_pair(uname, vname)) != edgeItems_.end())
    {
        return edgeItems_.find(std::make_pair(uname, vname))->second;
    }
    else if (!graph_->isDirected() && edgeItems_.find(std::make_pair(vname, uname)) != edgeItems_.end())
    {
        return edgeItems_.find(std::make_pair(vname, uname))->second;
    }

    return nullptr;
}

void GraphScene::clearAll(bool keepBackground)
{
    if (!nodeItems_.empty())
    {
        for (auto &nodeItem : nodeItems_)
        {
            nodeItem.second->deleteLater();
        }
        nodeItems_.clear();
    }

    if (!edgeItems_.empty())
    {
        for (auto &edgeItem : edgeItems_)
        {
            edgeItem.second->deleteLater();
        }
        edgeItems_.clear();
    }

    if (keepBackground)
    {
        foreach (QGraphicsItem *item, items())
        {
            if (dynamic_cast<QGraphicsPixmapItem *>(item) == nullptr)
            {
                removeItem(item);
                delete item;
            }
        }
    }
    else
    {
        clear();
    }
}

void GraphScene::onReload()
{
    clearAll(true);

    for (const auto &node : getGraph()->getNodes())
    {
        NodeItem *item = new NodeItem(this, node);
        nodeItems_[node->getName()] = item;
    }

    for (auto it = getGraph()->getEdges().begin(); it != getGraph()->getEdges().end(); ++it)
    {
        auto edge = graph::Edge(it);
        auto edgeItem = new EdgeItem(this, nodeItems_[edge.u()->getName()], nodeItems_[edge.v()->getName()]);
        edgeItems_.insert({std::make_pair(edge.u()->getName(), edge.v()->getName()), edgeItem});
    }

    for (const auto &item : edgeItems_)
    {
        addItem(item.second);
    }

    for (const auto &item : nodeItems_)
    {
        addItem(item.second);
    }

    update();
}

} // end namespace dcis::gui
