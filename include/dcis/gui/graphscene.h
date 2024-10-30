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
#ifndef DCIS_GUI_GRAPHSCENE_H_
#define DCIS_GUI_GRAPHSCENE_H_

// App includes
#include <graph/graph.h>
#include <gui/edgeitem.h>
#include <gui/nodeitem.h>

// Qt includes
#include <QGraphicsScene>
#include <QtWidgets>

namespace dcis::gui
{

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
  public:
    GraphScene() = default;
    explicit GraphScene(graph::Graph *graph);
    ~GraphScene() override;

    graph::Graph *getGraph() const;
    void setGraph(graph::Graph *graph);

    NodeItem *getNodeItem(const std::string &name);
    EdgeItem *getEdgeItem(const std::string &uname, const std::string &vname);

    void clearAll(bool keepBackground = false);

  public slots:
    void onReload();

  signals:
    void sigItemMoved();
    void sigNeedRedraw();
    void sigGraphChanged();

  private:
    using QGraphicsScene::clear;

    graph::Graph *graph_ = nullptr;
    std::unique_ptr<QTimer> uniqueTimer_;
    std::unordered_map<std::string, NodeItem *> nodeItems_;
    std::unordered_map<std::pair<std::string, std::string>, EdgeItem *> edgeItems_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHSCENE_H_
