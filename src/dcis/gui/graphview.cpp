#include <gui/graphview.h>


namespace dcis::gui {

GraphView::GraphView()
{
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GraphView::setScene(GraphScene* scene)
{
    connect(scene, &GraphScene::sigGraphChanged, this, &GraphView::onRedraw);
    connect(scene, &GraphScene::sigNeedRedraw, this, &GraphView::onRedraw);
    QGraphicsView::setScene(scene);
}

void GraphView::onRedraw()
{
    viewport()->update();
}

void GraphView::wheelEvent(QWheelEvent* event)
{
    QGraphicsView::wheelEvent(event);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

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

    if (!isSelectTargetNode)
    {
        QGraphicsView::mousePressEvent(event);
    }
    if (isSelectTargetNode)
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
    isSelectTargetNode = false;
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
            emit sigNodeSelected(nodeItem->getNode()->getName());
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
    if (!clickedItems.empty())
    {
        auto item = clickedItems[0];
        item->setSelected(true);

        auto* edgeItem = dynamic_cast<EdgeItem*>(item);
        auto* nodeItem = dynamic_cast<NodeItem*>(item);
        if (nodeItem)
        {
            auto nodeName = nodeItem->getNode()->getName();
            emit sigNodeSelected(nodeName);

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
                    isSelectTargetNode = true;
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
    else
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
    }
}

} // end namespace dcis::gui
