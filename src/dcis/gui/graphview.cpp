#include <gui/graphview.h>


namespace dcis::gui {

GraphView::GraphView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(ScrollHandDrag);
    setCacheMode(CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setViewportUpdateMode(FullViewportUpdate);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // When zooming, the view stay centered over the mouse
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    setStyleSheet("background-color: rgba(0, 0, 0, 0);");
}

void GraphView::setScene(GraphScene* scene)
{
    connect(scene, &GraphScene::sigGraphChanged, this, &GraphView::onRedraw);
    connect(scene, &GraphScene::sigNeedRedraw, this, &GraphView::onRedraw);
    connect(scene, &GraphScene::sigItemMoved, this, [this] {
        emit sigNodeMoved();
    });
    QGraphicsView::setScene(scene);
}

void GraphView::setSceneSize(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

void GraphView::onRedraw()
{
    viewport()->update();

}

void GraphView::wheelEvent(QWheelEvent* event)
{
    QGraphicsView::wheelEvent(event);

    double scaleFactor = 1.05;
    if (event->angleDelta().y() > 0 && currentScale_ <= scaleMax_)
    {
        scale(scaleFactor, scaleFactor);
        currentScale_ *= scaleFactor;
    }
    else
    {
        scale(1 / scaleFactor, 1 / scaleFactor);
        currentScale_ /= scaleFactor;
    }
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    setSceneRect(0, 0, event->size().width(), event->size().height());
    QGraphicsView::resizeEvent(event);
}

void GraphView::drawBackground(QPainter*, const QRectF&)
{
    // transparent background
}

void GraphView::mousePressEvent(QMouseEvent* event)
{
    for (auto item : scene()->selectedItems())
    {
        auto nodeItem = dynamic_cast<NodeItem*>(item);
        if (nodeItem)
        {
            nodeItem->setSelectedColor(NodeItem::getDefaultSelectedColor());
        }
        else
        {
            auto edgeItem = dynamic_cast<EdgeItem*>(item);
            if (edgeItem)
            {
                edgeItem->setSelectedColor(EdgeItem::getDefaultSelectedColor());
            }
        }
    }

    if (!isSelectTargetNode_)
    {
        QGraphicsView::mousePressEvent(event);
    }

    if (isSelectTargetNode_)
    {
        QList<QGraphicsItem*> itemsTo = items(event->pos());
        if (!itemsTo.empty())
        {
            auto* castedItemTo = dynamic_cast<NodeItem*>(itemsTo[0]);
            if (startItem_ && castedItemTo && startItem_ != castedItemTo)
            {
                castedItemTo->setSelected(false);
                emit sigEdgeSet(startItem_->getNode()->getName(), castedItemTo->getNode()->getName());
            }
        }
    }

    isSelectTargetNode_ = false;
    startItem_ = nullptr;

    scene()->update();
    viewport()->update();
}

void GraphView::mouseReleaseEvent(QMouseEvent* event)
{
    if (!scene()->selectedItems().empty())
    {
        auto nodeItem = dynamic_cast<NodeItem*>(scene()->selectedItems()[0]);
        auto edgeItem = dynamic_cast<EdgeItem*>(scene()->selectedItems()[0]);

        if (edgeItem)
        {
            emit sigEdgeSelected(edgeItem->getEdge().u()->getName(), edgeItem->getEdge().v()->getName());
        }
        else if (nodeItem)
        {
            QPointF pos = scene()->selectedItems()[0]->pos();
            emit sigNodeSelected(nodeItem->getNode()->getName(), pos);
        }
    }

    if (items(event->pos()).empty())
    {
        emit sigUnSelected();
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphView::contextMenuEvent(QContextMenuEvent* event)
{
    for (auto item : scene()->selectedItems())
    {
        item->setSelected(false);
    }

    QList<QGraphicsItem*> clickedItems = items(event->pos());

    if (clickedItems.empty() || (!clickedItems.empty() && dynamic_cast<QGraphicsPixmapItem*>(clickedItems[0])))
    {
        if (!scene()->selectedItems().empty())
        {
            scene()->selectedItems()[0]->setSelected(false);
        }

        QMenu menu;
        menu.addAction("Create node now");
        menu.addAction("Create node...");
        QAction *act = menu.exec(event->globalPos());
        if (act)
        {
            if (act->text() == "Create node...")
            {
               emit sigNodeAdded(mapToScene(event->pos()), false);
            }
            else if (act->text() == "Create node now")
            {
               emit sigNodeAdded(mapToScene(event->pos()), true);
            }
        }

        return;
    }

    if (!clickedItems.empty())
    {
        auto item = clickedItems[0];
        item->setSelected(true);

        auto* edgeItem = dynamic_cast<EdgeItem*>(item);
        auto* nodeItem = dynamic_cast<NodeItem*>(item);

        if (nodeItem)
        {
            auto nodeName = nodeItem->getNode()->getName();
            emit sigNodeSelected(nodeName, item->pos());

            QMenu menu;
            menu.addAction("&Set edge to (Select other node by mouse)");
            menu.addSeparator();
            menu.addAction("&Delete");
            menu.addAction("&Isolate");
            menu.addAction("Re&name");

            QAction* act = menu.exec(event->globalPos());
            if (act != nullptr)
            {
                if (act->text() == "Re&name")
                {
                    emit sigNodeEdited(nodeName);
                }
                if (act->text() == "&Isolate")
                {
                    emit sigNodeIsolated(nodeName);
                }
                if (act->text() == "&Delete")
                {
                    emit sigNodeRemoved(nodeName);
                }
                if (act->text().contains("&Set edge to"))
                {
                    isSelectTargetNode_ = true;
                    startItem_ = nodeItem;
                }
            }
            else
            {
                item->setSelected(false);
            }

        }
        else if (edgeItem)
        {
            QMenu menu;
            menu.addAction("&Delete");

            emit sigEdgeSelected(edgeItem->getEdge().u()->getName(), edgeItem->getEdge().v()->getName());

            QAction *act = menu.exec(event->globalPos());
            if (act != nullptr)
            {
                if (act->text() == "&Delete")
                {
                   emit sigEdgeRemoved(edgeItem->getEdge().u()->getName(), edgeItem->getEdge().v()->getName());
                }
            }
        }
    }
}

} // end namespace dcis::gui
