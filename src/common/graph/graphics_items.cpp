/*
 * Project Name: SWARM_X - System for Wireless Autonomous Resource Management and eXecution
 * Copyright (C) 2025 Artyom Lazyan
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <graph/graphics_items.h>

// Standard Library

// Qt Headers
#include <QWidget>
#include <QFontMetrics>
#include "graphics_items.h"

// Project Headers

namespace dcis::common::graph
{

NodeItemBase::NodeItemBase(Node *node, QGraphicsItem* parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setNode(node);
}

void NodeItemBase::setNode(graph::Node *node)
{
    node_ = node;
    setPos(node_->getEuclidePos());
    QFontMetrics fm(QFont(font_, fontSize_));

    radius_ = std::max(radius_, fm.horizontalAdvance(QString::fromStdString(node_->getName() + "  ")));
}

auto NodeItemBase::getRadius() const -> int
{
    return radius_;
}

void NodeItemBase::setRadius(int radius)
{
    radius_ = radius;
}

auto NodeItemBase::getColor() const -> QColor
{
    return color_;
}

void NodeItemBase::setColor(QColor color)
{
    color_ = color;
}

auto NodeItemBase::getNode() const -> Node *
{
    return node_;
}

auto NodeItemBase::boundingRect() const -> QRectF
{
    return {-radius_ / 2. + .5, -radius_ / 2. + .5, static_cast<qreal>(radius_ + 4), static_cast<qreal>(radius_ + 4)};
}

auto NodeItemBase::type() const -> int
{
    return Type;
}

void NodeItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

EdgeItemBase::EdgeItemBase(NodeItemBase *startItem, NodeItemBase *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), endItem_(endItem), startItem_(startItem)
{
}

auto EdgeItemBase::getColor() const -> QColor
{
    return color_;
}

void EdgeItemBase::setColor(QColor color)
{
    color_ = color;
}

QPainterPath EdgeItemBase::shape() const
{
    return path_;
}

QRectF EdgeItemBase::boundingRect() const
{
    qreal extra = (pen().width() + 30) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void EdgeItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (startItem_->collidesWithItem(endItem_))
    {
        return;
    }

    QColor color;
    painter->setTransform(transform(), true);

    color = getColor();
    QPen myPen = pen();
    myPen.setColor(color);
    myPen.setWidth(4);
    painter->setBrush(color);
    painter->setPen(myPen);
    painter->drawPath(shape());
}

} // end namespace swrm_x::graph
