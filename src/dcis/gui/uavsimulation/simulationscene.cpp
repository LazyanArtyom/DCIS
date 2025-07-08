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
#include <gui/uavsimulation/simulationscene.h>

// Qt includes
#include <QPropertyAnimation>

namespace dcis::gui
{

UAVSimulationScene::UAVSimulationScene(QObject* parent) : QGraphicsScene(parent) {}

UAVSimulationScene::~UAVSimulationScene() 
{
    for (auto &entry : trajectoryItems_)
    {
        removeItem(entry.second);
        delete entry.second;
    }
   
    trajectoryItems_.clear();
    trajectories_.clear();
}

auto UAVSimulationScene::getNodeItem(const std::string &name) -> common::graph::NodeItemBase *
{
    auto nodeIt = nodeItems_.find(name);
    return (nodeIt != nodeItems_.end()) ? nodeIt->second : nullptr;
}

void UAVSimulationScene::addNodeItem(const std::string &name, common::graph::NodeItemBase *nodeItem)
{
    nodeItems_.insert({name, nodeItem});
}

void UAVSimulationScene::testAnimation()
{
    static int globalStep = 0;
    const int maxStep = 200;

    for (const auto& [name, node] : nodeItems_)
    {
        QPointF currentPos = node->pos();

        // Create a per-node hash to seed movement
        std::hash<std::string> hasher;
        size_t seed = hasher(name) + globalStep * 7919; // Large prime for chaos

        // Use hash to pseudo-randomly pick direction
        int dx = static_cast<int>((seed >> 2) % (2 * maxStep + 1)) - maxStep;
        int dy = static_cast<int>((seed >> 5) % (2 * maxStep + 1)) - maxStep;

        // Avoid zero movement
        if (dx == 0 && dy == 0) dx = 50;

        QPointF newPos = currentPos + QPointF(dx, dy);
        animateNodeMovement(node, newPos);
    }

    ++globalStep;
}

void UAVSimulationScene::clearAll(bool keepBackground)
{
    if (!nodeItems_.empty())
    {
        for (auto &nodeItem : nodeItems_)
        {
            nodeItem.second->deleteLater();
        }
        nodeItems_.clear();
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

void UAVSimulationScene::updateGraph(common::graph::Graph *newGraph)
{
   /* clearAll(true);

    for (const auto& node : newGraph->getNodes())
    {
        if (node->getCategory() != common::graph::Node::Category::Drone) 
        {
            continue;
        }

        auto nodeItem = new common::graph::NodeItemBase(node);
        nodeItem->setRadius(40);

        addItem(nodeItem);
        nodeItems_[node->getName()] = nodeItem;
    }*/
    for (const auto& node : newGraph->getNodes())
    {
        if (node->getCategory() != common::graph::Node::Category::Drone) 
        {
            continue;
        }

        if (auto *nodeItem = getNodeItem(node->getName()))
        {
            QPointF newPos(node->getX(), node->getY());
            animateNodeMovement(nodeItem, newPos);

            if (trajectories_.find(node->getName()) == trajectories_.end())
            {
                QPainterPath path(nodeItem->pos());
                trajectories_[node->getName()] = path;

                auto *pathItem = new QGraphicsPathItem(path);
                QPen pen(Qt::red, 3);
                pathItem->setPen(pen);
                addItem(pathItem);
                trajectoryItems_[node->getName()] = pathItem;
            }
        }
        else
        {
            nodeItem = new common::graph::NodeItemBase(node);
            nodeItem->setRadius(40);

            addItem(nodeItem);
            nodeItems_[node->getName()] = nodeItem;
        }
    }
}

void UAVSimulationScene::animateNodeMovement(common::graph::NodeItemBase *node, const QPointF &newPos)
{
    const std::string name = node->getNode()->getName();
    QPointF startPos = node->pos();

    if (trajectories_.find(name) == trajectories_.end())
    {
        QPainterPath path(startPos);
        trajectories_[name] = path;

        auto *pathItem = new QGraphicsPathItem(path);
        QPen pen(Qt::red, 3); // You can style it as needed
        pathItem->setPen(pen);
        addItem(pathItem);

        trajectoryItems_[name] = pathItem;
    }

    // Create animation
    auto *animation = new QPropertyAnimation(node, "pos");
    animation->setDuration(7000);  // in ms
    animation->setStartValue(startPos);
    animation->setEndValue(newPos);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::valueChanged, this, [this, name](const QVariant &value) {
        QPointF currentPos = value.toPointF();
        trajectories_[name].lineTo(currentPos);
        trajectoryItems_[name]->setPath(trajectories_[name]);
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

} // namespace dcis::gui
