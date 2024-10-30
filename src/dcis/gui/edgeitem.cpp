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
#include <gui/edgeitem.h>

// APP includes
#include <gui/graphscene.h>

// QT includes
#include <QPen>
#include <QList>
#include <QCursor>
#include <QPainter>

namespace dcis::gui
{

EdgeItem::EdgeItem(GraphScene *scene, NodeItem *startItem, NodeItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), endItem_(endItem), gscene_(scene), startItem_(startItem)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    onUpdatePosition();
    connect(startItem_, &NodeItem::sigPositionChanged, this, &EdgeItem::onUpdatePosition);
    connect(endItem_, &NodeItem::sigPositionChanged, this, &EdgeItem::onUpdatePosition);
    emit gscene_->sigNeedRedraw();
}

QColor EdgeItem::getDefaultColor()
{
    return Qt::red;
}

QColor EdgeItem::getDefaultSelectedColor()
{
    return Qt::darkRed;
}

int EdgeItem::type() const
{
    return Type;
}

QPainterPath EdgeItem::shape() const
{
    return path_;
}

QRectF EdgeItem::boundingRect() const
{
    qreal extra = (pen().width() + 30) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

graph::Edge EdgeItem::getEdge() const
{
    return gscene_->getGraph()->getEdge(startItem_->getNode(), endItem_->getNode());
}

bool EdgeItem::isInversionAvailable() const
{
    return gscene_->getGraph()->hasEdge(getEdge().v(), getEdge().u());
}

void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (startItem_->collidesWithItem(endItem_))
    {
        return;
    }

    QColor color;
    painter->setTransform(transform(), true);
    // updatePosition();
    if (isSelected())
    {
        color = getDefaultSelectedColor();
    }
    else
    {
        color = getDefaultColor();
    }

    QPen myPen = pen();
    myPen.setColor(color);
    myPen.setWidth(4);
    painter->setBrush(color);
    painter->setPen(myPen);
    painter->drawPath(shape());
}

void EdgeItem::onUpdatePosition()
{
    qreal p = startItem_->pos().x();
    qreal q = startItem_->pos().y();
    qreal r = endItem_->pos().x();
    qreal s = endItem_->pos().y();
    qreal length_x = qFabs(p - r);
    qreal length_y = qFabs(q - s);
    qreal length = qSqrt(length_x * length_x + length_y * length_y);
    qreal tx1 = 1 - (endItem_->getRadius() / 2.) / length_x;
    qreal ty1 = 1 - (endItem_->getRadius() / 2.) / length_y;

    QPointF centerPos = QPoint(static_cast<int>((p + r) / 2), static_cast<int>((q + s) / 2));

    QLineF centerLine(mapFromItem(startItem_, 0, 0), mapFromItem(endItem_, 0, 0));
    setLine(centerLine);
    path_ = QGraphicsLineItem::shape();

    QPointF arrowHeadPoint((1 - tx1) * p + tx1 * r, (1 - tx1) * q + tx1 * s);
    qreal arrowHeadSize = 15;
    qreal angle = std::atan2(line().dy(), -line().dx());

    if (std::abs(angle * 180 / M_PI) >= 45 && std::abs(angle * 180 / M_PI) <= 135)
    {
        arrowHeadPoint = QPointF((1 - ty1) * p + ty1 * r, (1 - ty1) * q + ty1 * s);
    }

    if (gscene_->getGraph()->isDirected() && isInversionAvailable())
    {
        qreal offset = arrowHeadSize / 2;
        qreal foo = qMax(5., qMin(25., length / 6));
        if (getEdge().u()->getName() < getEdge().v()->getName())
        {
            centerPos += QPointF(-foo, -foo);
            setLine(QLineF(line().p1(), line().p2() + QPointF(-4, -4)));
            arrowHeadPoint += QPointF(-offset, -offset);
        }
        else
        {
            centerPos += QPointF(foo, foo);
            setLine(QLineF(line().p1(), line().p2() + QPointF(4, 4)));
            arrowHeadPoint += QPointF(offset, offset);
        }

        QPainterPath tempPath(line().p1());
        tempPath.cubicTo(centerPos, centerPos, line().p2());
        QPainterPathStroker pathStroke;
        QPainterPath outline = pathStroke.createStroke(tempPath);
        path_ = outline;
    }

    if (gscene_->getGraph()->isDirected())
    {
        QPointF arrowP1 =
            arrowHeadPoint + QPointF(sin(angle + M_PI / 3) * arrowHeadSize, cos(angle + M_PI / 3) * arrowHeadSize);
        QPointF arrowP2 = arrowHeadPoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowHeadSize,
                                                   cos(angle + M_PI - M_PI / 3) * arrowHeadSize);

        QPolygonF arrowHead;
        arrowHead << arrowHeadPoint << arrowP2 << arrowP1;
        path_.addPolygon(arrowHead);
    }

    emit gscene_->sigNeedRedraw();
}

} // end namespace dcis::gui
