#include <gui/uavsimulation/simulationscene.h>

#include <QPropertyAnimation>

namespace dcis::gui
{

NodeItem* UAVSimulationScene::getNodeItem(const std::string &name)
{
    auto it = nodeItems_.find(name);
    return (it != nodeItems_.end()) ? it->second : nullptr;
}

void UAVSimulationScene::addNodeItem(const std::string &name, NodeItem *nodeItem)
{
    nodeItems_.insert({name, nodeItem});
}

void UAVSimulationScene::updateGraph(graph::Graph *newGraph)
{
    if (!newGraph)
        return;

    /*for (graph::Node* node : newGraph->getNodes())
    {
        if (auto nodeItem = getNodeItem(node->getName()))
        {
            QPointF newPos(node->getX(), node->getY());
            animateNodeMovement(nodeItem, newPos);
            trajectories_[node->getName()].append(newPos);
        }
        else
        {
            nodeItem = new NodeItem(this, node);

            addItem(nodeItem);
            nodeItems_[name] = nodeItem;
        }
    }*/
}

void UAVSimulationScene::animateNodeMovement(NodeItem *node, const QPointF &newPos)
{
    auto animation = std::make_unique<QPropertyAnimation>(node, "pos");
    animation->setDuration(1000);
    animation->setStartValue(node->pos());
    animation->setEndValue(newPos);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

} // namespace dcis::gui