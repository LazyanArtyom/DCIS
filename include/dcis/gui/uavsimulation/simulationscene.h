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
#ifndef DCIS_GUI_SIMULATION_SCENE_H_
#define DCIS_GUI_SIMULATION_SCENE_H_

// App includes
#include <graph/graph.h>
#include <graph/graphics_items.h>

// Qt includes
#include <QGraphicsScene>

namespace dcis::gui
{

class UAVSimulationScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit UAVSimulationScene(QObject* parent = nullptr);
    ~UAVSimulationScene() override;

    void clearAll(bool keepBackground = false);

    void updateGraph(common::graph::Graph *newGraph);
    common::graph::NodeItemBase* getNodeItem(const std::string &name);
    void addNodeItem(const std::string &name, common::graph::NodeItemBase *nodeItem);

    void testAnimation();

private:
    void animateNodeMovement(common::graph::NodeItemBase *node, const QPointF &newPos);

    std::unordered_map<std::string, common::graph::NodeItemBase *> nodeItems_;
    std::unordered_map<std::string, QGraphicsPathItem*> trajectoryItems_;
    std::unordered_map<std::string, QPainterPath> trajectories_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_SIMULATION_SCENE_H_
