#include <gui/edgeitem.h>

// APP includes
#include <gui/graphscene.h>

// QT includes
#include <QPen>
#include <QList>
#include <QCursor>
#include <QPainter>

namespace dcis::gui {

EdgeItem::EdgeItem(GraphScene* scene, NodeItem* startItem, NodeItem* endItem, QColor color, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), gscene_(scene), startItem_(startItem), endItem_(endItem), color_(color)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setPen(QPen(color_, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    selectedColor_ = getColorTable()[1];

    onUpdatePosition();
    connect(startItem_, &NodeItem::sigPositionChanged, this, &EdgeItem::onUpdatePosition);
    connect(endItem_, &NodeItem::sigPositionChanged, this, &EdgeItem::onUpdatePosition);
    emit gscene_->sigNeedRedraw();
}

QColor EdgeItem::getDefaultColor()
{
    return getColorTable()[0];
}

QColor EdgeItem::getDefaultSelectedColor()
{
    return getColorTable()[1];
}

const QList<QColor>& EdgeItem::getColorTable()
{
    static QList<QColor> colorTable;
    colorTable.append(QColor(57, 89, 119));
    colorTable.append(QColor(205, 92, 92));
    colorTable.append(QColor(57, 89, 119));
    colorTable.append(QColor(85, 107, 47));
    colorTable.append(QColor(47, 79, 79));
    colorTable.append(QColor(119, 136, 153));
    colorTable.append(QColor(112, 128, 144));

    return colorTable;
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
    qreal extra = ( pen().width() + 30 ) / 2.0;

    return QRectF( line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                       line().p2().y() - line().p1().y()) )
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

void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsItem::hoverEnterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (startItem_->collidesWithItem(endItem_))
    {
        return;
    }

    painter->setTransform(transform(), true);
    //updatePosition();
    if (isSelected())
    {
        color_ = getSelectedColor();
    }
    else
    {
        color_ = getDefaultColor();
    }

    QPen myPen = pen();
    myPen.setColor(color_);
    myPen.setWidth(4);
    painter->setBrush(color_);
    painter->setPen(myPen);
    painter->drawPath(shape());
}

QColor EdgeItem::getSelectedColor() const
{
    return selectedColor_;
}

void EdgeItem::setSelectedColor(QColor color)
{
    selectedColor_ = color;
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

    QPointF centerPos = QPoint(
            static_cast<int>((p + r) / 2),
            static_cast<int>((q + s) / 2));

    QLineF centerLine(mapFromItem(startItem_, 0, 0), mapFromItem(endItem_, 0, 0));
    setLine(centerLine);
    path_ = QGraphicsLineItem::shape();

    QPointF arrowHeadPoint((1 - tx1) * p + tx1 * r, (1 - tx1) * q + tx1 * s);
    qreal arrowHeadSize = 15;
    qreal angle = std::atan2(line().dy(), -line().dx());

    if (std::abs(angle * 180 / M_PI) >= 45 && std::abs(angle * 180 / M_PI) <= 135)
    {
        arrowHeadPoint = QPointF(
                (1 - ty1) * p + ty1 * r,
                (1 - ty1) * q + ty1 * s);
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
        QPointF arrowP1 = arrowHeadPoint + QPointF(sin(angle + M_PI / 3) * arrowHeadSize,
                                                   cos(angle + M_PI / 3) * arrowHeadSize);
        QPointF arrowP2 = arrowHeadPoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowHeadSize,
                                                   cos(angle + M_PI - M_PI / 3) * arrowHeadSize);

        QPolygonF arrowHead;
        arrowHead << arrowHeadPoint << arrowP2 << arrowP1;
        path_.addPolygon(arrowHead);
    }

    emit gscene_->sigNeedRedraw();
}

} // end namespace dcis::gui
