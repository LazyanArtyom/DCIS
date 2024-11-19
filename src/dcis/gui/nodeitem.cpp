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
#include <gui/nodeitem.h>

// APP includes
#include <gui/graphscene.h>

// QT includes
#include <QDebug>
#include <QtWidgets>

namespace dcis::gui
{

NodeItem::NodeItem(GraphScene *gscene, graph::Node *node)
{
    gscene_ = gscene;
    radius_ = getDefaultRadius();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setNode(node);
    isMoving_ = false;
}

void NodeItem::setNode(graph::Node *node)
{
    node_ = node;
    setPos(node_->getEuclidePos());
    QFontMetrics fm(QFont(font_, fontSize_));

    radius_ = std::max(radius_, fm.horizontalAdvance(QString::fromStdString(node_->getName() + "  ")));
}

graph::Node *NodeItem::getNode() const
{
    return node_;
}

int NodeItem::getRadius() const
{
    return radius_;
}

QRectF NodeItem::boundingRect() const
{
    return {-radius_ / 2. + .5, -radius_ / 2. + .5, static_cast<qreal>(radius_ + 4), static_cast<qreal>(radius_ + 4)};
}

int NodeItem::getDefaultRadius()
{
    return 40;
}

QColor NodeItem::getDefaultColor()
{
    return Qt::green;
}

QColor NodeItem::getDefaultSelectedColor()
{
    return Qt::darkGreen;
}

int NodeItem::type() const
{
    return Type;
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (isSelected())
    {
        painter->setOpacity(0.5);
    }
    else
    {
        painter->setOpacity(1);
    }

    if (node_ != nullptr)
    {
        switch (node_->getCategory())
        {
            case graph::Node::Category::Drone: 
            {
                QPixmap iconPixmap(":/resources/set_drone.png");
                painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
                return;
            }
            case graph::Node::Category::Attacker: 
            {
                QPixmap iconPixmap(":/resources/set_attacker.png");
                painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
                return;
            }
            case graph::Node::Category::Target: 
            {
                QPixmap iconPixmap(":/resources/set_target.svg");
                painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
                return;
            }
            default:
                break;
        }

        switch (node_->getType())
        {
        case graph::Node::Type::Border: {
            QPixmap iconPixmap(":/resources/set_border.png");
            painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
            return;
        }
        case graph::Node::Type::Corner: {
            QPixmap iconPixmap(":/resources/set_corner.png");
            painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
            return;
        }
        case graph::Node::Type::Inner: {
            QPixmap iconPixmap(":/resources/set_inner.png");
            painter->drawPixmap(-radius_ / 2, -radius_ / 2, radius_, radius_, iconPixmap);
            return;
        }
        default:
            break;
        }
    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() <
        QApplication::startDragDistance())
    {
        return;
    }

    isMoving_ = true;
    setCursor(Qt::ClosedHandCursor);
    setPos(event->scenePos());
    getNode()->setEuclidePos(pos());

    emit gscene_->sigNeedRedraw();
    emit sigPositionChanged();

    QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if (isMoving_)
    {
        setSelected(false);
        isMoving_ = false;

        emit gscene_->sigItemMoved();
    }
    // emit this->_gscene->graphChanged();
    setCursor(Qt::OpenHandCursor);
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

} // end namespace dcis::gui
