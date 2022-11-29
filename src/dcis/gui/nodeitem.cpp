#include <gui/nodeitem.h>

// App includes
//#include "graphics/headers/GraphGraphicsScene.h"

// Qt includes
#include <QDebug>
#include <utility>
#include <QtWidgets>

namespace dcis::gui {

NodeItem::NodeItem(graph::Node* node, QColor color)
{
    color_ = std::move(color);
    radius_ = 80;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setNode(node);
    isMoving_ = false;
    selectedColor_ = getDefaultOnSelectedColor();
}

void NodeItem::setNode(graph::Node* node)
{
    node_ = node;
    setPos(node_->getEuclidePos());
    QFontMetrics fm(QFont(font_, fontSize_));
    radius_ = std::max(radius_, fm.horizontalAdvance(QString::fromStdString(node_->getName() + "  ")));
}

graph::Node* NodeItem::getNode() const
{
    return node_;
}

int NodeItem::getRadius() const
{
    return radius_;
}

QRectF NodeItem::boundingRect() const
{
    return
    {
        -radius_ / 2. + .5,
        -radius_ / 2. + .5,
        static_cast<qreal>(radius_ + 4),
        static_cast<qreal>(radius_ + 4)
    };
}

const QList<QColor> &NodeItem::getColorTable()
{
    static QList<QColor> colorTable = {
        QColor(244, 164, 96),
        QColor(50, 205, 50),
        QColor(230, 230, 250),
        QColor(244, 164, 96),
        QColor(245, 222, 179),
        QColor(221, 160, 221),
        QColor(135, 206, 235),
        QColor(240, 230, 140),
        QColor(255, 165, 0),
        QColor(240, 255, 255),
        QColor(0, 255, 255),
        QColor(255, 255, 0),
        QColor(240, 255, 255),
        QColor(255, 228, 225),
        QColor(220, 220, 220),
        QColor(176, 196, 222)
    };

    return colorTable;
};

QColor NodeItem::getDefaultColor()
{
    return getColorTable()[0];
}

QColor NodeItem::getDefaultOnSelectedColor()
{
    return getColorTable()[1];
}

int NodeItem::type() const
{
    return Type;
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (isSelected())
    {
        color_ = getSelectedColor();
    }
    else
    {
        color_ = getDefaultColor();
    }

    painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter->setBrush(QBrush(color_));
    painter->drawEllipse(-radius_ / 2, -radius_ / 2, radius_, radius_);

    QFont font(font_, fontSize_);
    painter->setFont(font);
    QString txt = QString::fromStdString(getNode()->getName());
    QFontMetrics fm(font);
    painter->drawText(-fm.horizontalAdvance(txt) / 2, fm.height() / 3, txt);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
                .length() < QApplication::startDragDistance())
    {
        return;
    }

    isMoving_ = true;
    setCursor(Qt::ClosedHandCursor);
    setPos(event->scenePos());
    getNode()->setEuclidePos(pos());

    /**********************/
    //emit gscene_->needRedraw();
    emit sigNeedRedraw();
    emit sigPositionChanged();
    QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if (isMoving_) {
        setSelected(false);
        isMoving_ = false;
    }
    //emit this->_gscene->graphChanged();
    setCursor(Qt::OpenHandCursor);
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsItem::hoverEnterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

QColor NodeItem::getSelectedColor() const
{
    return selectedColor_;
}

QColor NodeItem::getColor() const
{
    return color_;
}

void NodeItem::setColor(const QColor& newColor)
{
    color_ = newColor;
}

void NodeItem::setOnSelectedColor(const QColor& newColor)
{
    selectedColor_ = newColor;
}

} // end namespace dcis::gui
