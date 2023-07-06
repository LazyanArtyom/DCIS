#include <gui/nodeitem.h>

// APP includes
#include <gui/graphscene.h>

// QT includes
#include <QDebug>
#include <utility>
#include <QtWidgets>

namespace dcis::gui {

NodeItem::NodeItem(gui::GraphScene* gscene, graph::Node* node)
{
    gscene_ = gscene;
    radius_ = getDefaultRadius();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
    setNode(node);
    isMoving_ = false;
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

int NodeItem::getDefaultRadius()
{
    return 20;
}

QColor NodeItem::getDefaultColor()
{
    if (node_ != nullptr)
    {
        if (node_->isDrone())
        {
            return Qt::cyan;
        }

        switch (node_->getNodeType()) {
            case graph::Node::NodeType::Border:
                return Qt::magenta;
            case graph::Node::NodeType::Corner:
                return Qt::yellow;
            case graph::Node::NodeType::Inner:
                return Qt::green;
            default:
                return Qt::green;
        }
    }
    return Qt::green;
}

QColor NodeItem::getDefaultSelectedColor()
{
    if (node_ != nullptr)
    {
        if (node_->isDrone())
        {
            return Qt::darkCyan;
        }

        switch (node_->getNodeType()) {
            case graph::Node::NodeType::Border:
                return Qt::darkMagenta;
            case graph::Node::NodeType::Corner:
                return Qt::darkYellow;
            case graph::Node::NodeType::Inner:
                return Qt::darkGreen;
            default:
                return Qt::darkGreen;
        }
    }

    return Qt::darkGreen;
}

int NodeItem::type() const
{
    return Type;
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color;
    if (isSelected())
    {
        color = getDefaultSelectedColor();
    }
    else
    {
        color = getDefaultColor();
    }

    painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter->setBrush(QBrush(color));
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

    emit gscene_->sigNeedRedraw();
    emit gscene_->sigItemMoved();
    emit sigPositionChanged();

    QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if (isMoving_)
    {
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

} // end namespace dcis::gui
