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
#include <gui/graphview.h>

#include <gui/coordinputdialog.h>
#include <utils/debugstream.h>

#include <iostream>
#include <sstream>
#include <string>

namespace dcis::gui
{

GraphView::GraphView(QWidget *parent) : QGraphicsView(parent)
{
    setDragMode(ScrollHandDrag);
    setCacheMode(CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    // setViewportUpdateMode(FullViewportUpdate);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // When zooming, the view stay centered over the mouse
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    setStyleSheet("background-color: rgba(0, 0, 0, 0);");

    setBackgroundBrush(Qt::transparent); // Set transparent background

    // graph view widget
    graph_ = new graph::Graph(false);
    graphScene_ = new GraphScene(graph_);
    setScene(graphScene_);

    // connections
    connect(this, &GraphView::sigNodeSelected, this, [this](const std::string &nodeName, QPointF pos) {
        // txtConsole_->setText(txt);
    });
    connect(this, &GraphView::sigNodeAdded, this, [this](QPointF pos, bool autoNaming) {
        if (!autoNaming)
        {
            showNewNodeDialog(pos);
            return;
        }

        graph_->addNode(graph::Node(graph_->getNextNodeName(), pos));
        emit sigGraphChanged();
    });
    connect(this, &GraphView::sigNodeRemoved, this, [this](const std::string &nodeName) {
        if (graph_->removeNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });
    connect(this, &GraphView::sigNodeIsolated, this, [this](const std::string &nodeName) {
        if (graph_->isolateNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });
    connect(this, &GraphView::sigEdgeRemoved, this, [this](const std::string &uname, const std::string &vname) {
        if (graph_->removeEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });
    connect(this, &GraphView::sigEdgeSet, this, [this](const std::string &uname, const std::string &vname) {
        if (graph_->setEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });
    connect(this, &GraphView::sigNodeEdited, this, [this](const std::string &nodeName) {
        bool ok;
        QRegularExpression re(QRegularExpression::anchoredPattern(QLatin1String("[a-zA-Z0-9]{1,30}")));

        auto newName = QInputDialog::getText(this, tr("Rename node"), "Name: ", QLineEdit::Normal,
                                             QString::fromStdString(graph_->getNextNodeName()), &ok);
        if (ok)
        {
            static QRegularExpressionMatch match = re.match(newName);
            if (!match.hasMatch())
            {
                QMessageBox::critical(this, tr("Error"),
                                      tr("Node's name contains only alphabetical or numeric characters\n") +
                                          tr("Length of the name mustn't be greater than 30 or smaller than 1"));
                return;
            }
            if (graph_->hasNode(newName.toStdString()))
            {
                QMessageBox::critical(this, tr("Error"), tr("This name has been used by another node"));
            }
            else
            {
                graph_->setNodeName(nodeName, newName.toStdString());
                emit sigGraphChanged();
            }
        }
    });
    connect(this, &GraphView::sigSetNodeType, this,
            [this](const std::string &nodeName, const graph::Node::Type &type) {
                if (graph_->getNode(nodeName))
                {
                    graph_->getNode(nodeName)->setType(type);
                    emit sigGraphChanged();
                }
            });
    connect(this, &GraphView::sigSetNodeCategory, this,
            [this](std::string nodeName, graph::Node::Category nodeCategory, 
                   std::string ip, std::string port) {
                if (graph_->getNode(nodeName))
                {
                    graph_->getNode(nodeName)->setCategory(nodeCategory);
                    graph_->getNode(nodeName)->setIp(ip);
                    graph_->getNode(nodeName)->setPort(port);
                    emit sigGraphChanged();
                }
            });
}

void GraphView::setScene(GraphScene *scene)
{
    connect(scene, &GraphScene::sigGraphChanged, this, &GraphView::onRedraw);
    connect(scene, &GraphScene::sigNeedRedraw, this, &GraphView::onRedraw);
    connect(scene, &GraphScene::sigItemMoved, this, [this] { emit sigNodeMoved(); });

    connect(this, &GraphView::sigGraphChanged, scene, &GraphScene::onReload);
    QGraphicsView::setScene(scene);
}

void GraphView::setSceneSize(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

void GraphView::generateGraph(int row, int col)
{
    graph_->clear();

    int rowOffset = getImageInfo().imageSize.width() / (row - 1);
    int colOffset = getImageInfo().imageSize.height() / (col - 1);

    int rows = getImageInfo().imageSize.width();
    int cols = getImageInfo().imageSize.height();

    for (int row = 0, i = 0; row <= rows; row += rowOffset, ++i)
    {
        for (int col = 0, j = 0; col <= cols; col += colOffset, ++j)
        {
            std::stringstream nodeName;
            nodeName << "N" << i << ":" << j;
            graph_->addNode(graph::Node(nodeName.str(), QPointF(row, col)));
            qDebug() << "Added: " << nodeName.str();
        }
    }

    for (int row = 0, i = 0; row <= rows; row += rowOffset, ++i)
    {
        for (int col = 0, j = 0; col <= cols; col += colOffset, ++j)
        {
            if (row < rows)
            {
                std::stringstream edgeName1, edgeName2;
                edgeName1 << "N" << i << ":" << j;
                edgeName2 << "N" << (i + 1) << ":" << j;
                qDebug() << "setEdge: " << edgeName1.str() << " | " << edgeName2.str();
                graph_->setEdge(edgeName1.str(), edgeName2.str());
            }
            if (col < cols)
            {
                std::stringstream edgeName1, edgeName2;
                edgeName1 << "N" << i << ":" << j;
                edgeName2 << "N" << i << ":" << (j + 1);
                qDebug() << "setEdge: " << edgeName1.str() << " | " << edgeName2.str();
                graph_->setEdge(edgeName1.str(), edgeName2.str());
            }
        }
    }

    emit sigGraphChanged();
}

void GraphView::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphView::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void GraphView::viewFit()
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    isResized_ = true;

    if (sceneRect().width() > sceneRect().height())
        isLandscape_ = true;
    else
        isLandscape_ = false;
}

void GraphView::scaleView(qreal scaleFactor)
{
    if (sceneRect().isEmpty())
        return;

    int imgLength;
    int viewportLength;
    qreal expRectLength;
    QRectF rectExpectedRect = transform().scale(scaleFactor, scaleFactor).mapRect(sceneRect());

    if (isLandscape_)
    {
        expRectLength = rectExpectedRect.width();
        viewportLength = viewport()->rect().width();
        imgLength = sceneRect().width();
    }
    else
    {
        expRectLength = rectExpectedRect.height();
        viewportLength = viewport()->rect().height();
        imgLength = sceneRect().height();
    }

    if (expRectLength < qreal(viewportLength) / 2) // minimum zoom : half of viewport
    {
        if (!isResized_ || scaleFactor < 1)
            return;
    }
    else if (expRectLength > imgLength * 10) // maximum zoom : x10
    {
        if (!isResized_ || scaleFactor > 1)
            return;
    }
    else
    {
        isResized_ = false;
    }

    scale(scaleFactor, scaleFactor);
}

graph::Graph *GraphView::getGraph() const
{
    return graph_;
}

void GraphView::updateGraph(graph::Graph *graph)
{
    graph_ = graph;
    graphScene_->setGraph(graph);
}

GraphView::ImageInfo GraphView::getImageInfo()
{
    imageInfo_.sceneRectSize = QSizeF(scene()->sceneRect().width(), scene()->sceneRect().height());
    imageInfo_.viewportSize = viewport()->size();

    return imageInfo_;
}

void GraphView::setImage(const QImage &img)
{
    if (!scene()->sceneRect().isEmpty())
        scene()->clear();

    imageInfo_.imageSize = QSizeF(img.width(), img.height());
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));

    scene()->addItem(pixmapItem);

    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    pixmapItem->setPos(scene()->width() / 2 - pixmapItem->boundingRect().width() / 2,
                       scene()->height() / 2 - pixmapItem->boundingRect().height() / 2);
}

void GraphView::showNewNodeDialog(QPointF pos)
{
    bool ok;
    QRegularExpression re(QRegularExpression::anchoredPattern(QLatin1String("[a-zA-Z0-9]{1,3}")));

    QString newNodeName = QInputDialog::getText(this, "Add new node", "Name: ", QLineEdit::Normal,
                                                QString::fromStdString(graph_->getNextNodeName()), &ok);
    if (ok)
    {
        static QRegularExpressionMatch match = re.match(newNodeName);
        if (!match.hasMatch())
        {
            QMessageBox::critical(this, "Error",
                                  tr("Node's name contains only alphabetical or numeric characters\n") +
                                      tr("Length of the name mustn't be greater than 3 or smaller than 1"));
            return;
        }

        graph::Node newNode(newNodeName.toStdString(), pos);
        bool succeeded = graph_->addNode(newNode);
        if (!succeeded)
        {
            QMessageBox::critical(this, "Error", "This name has been used by another node");
        }
        else
        {
            emit sigGraphChanged();
        }
    }
}

void GraphView::onRedraw()
{
    viewport()->update();
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
        viewFit();
    }
    else if (event->modifiers() == Qt::NoModifier)
    {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    isResized_ = true;
    QGraphicsView::resizeEvent(event);
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    // Get the mouse click position in view coordinates
    QPoint viewPos = event->pos();

    // Get the corresponding scene coordinates
    QPointF scenePos = mapToScene(viewPos);

    // Get the x and y coordinates
    qreal x = scenePos.x();
    qreal y = scenePos.y();

    if (scene()->sceneRect().contains(scenePos))
    {
        QString mymsg = "Clicked at x:" + QString::number(x) + "y:" + QString::number(y);
        // utils::DebugStream::getInstance().log(utils::LogLevel::Info, mymsg);
    }
    else
    {
    }

    if (!scene()->selectedItems().isEmpty())
    {
        QString msg3 = "Node selected X: " + QString::number(scene()->selectedItems()[0]->pos().x()) +
                       " Y: " + QString::number(scene()->selectedItems()[0]->pos().y());
        // utils::DebugStream::getInstance().log(utils::LogLevel::Info, msg3);
    }

    if (!isSelectTargetNode_)
    {
        QGraphicsView::mousePressEvent(event);
    }

    if (isSelectTargetNode_)
    {
        QList<QGraphicsItem *> itemsTo = items(event->pos());
        if (!itemsTo.empty())
        {
            auto *castedItemTo = dynamic_cast<NodeItem *>(itemsTo[0]);
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

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!scene()->selectedItems().empty())
    {
        auto nodeItem = dynamic_cast<NodeItem *>(scene()->selectedItems()[0]);
        auto edgeItem = dynamic_cast<EdgeItem *>(scene()->selectedItems()[0]);

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

void GraphView::contextMenuEvent(QContextMenuEvent *event)
{
    for (auto item : scene()->selectedItems())
    {
        item->setSelected(false);
    }

    QList<QGraphicsItem *> clickedItems = items(event->pos());

    if (clickedItems.empty() || (!clickedItems.empty() && dynamic_cast<QGraphicsPixmapItem *>(clickedItems[0])))
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
            if (scene()->items().empty())
            {
                QMessageBox::warning(this, tr("Image is not loaded"),
                                     tr("Can't create node without loading an image."));
                return;
            }

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

        auto *edgeItem = dynamic_cast<EdgeItem *>(item);
        auto *nodeItem = dynamic_cast<NodeItem *>(item);

        if (nodeItem)
        {
            auto nodeName = nodeItem->getNode()->getName();
            emit sigNodeSelected(nodeName, item->pos());

            QMenu menu;
            menu.addAction("&Set edge to (Select other node by mouse)");
            menu.addSeparator();
            menu.addAction("&Set node corner");
            menu.addAction("&Set node border");
            menu.addAction("&Set node inner");
            menu.addSeparator();
            menu.addAction("&Delete");
            menu.addAction("&Isolate");
            menu.addAction("Re&name");
            menu.addSeparator();

            if (nodeItem->getNode()->getCategory() == graph::Node::Category::Drone)
            {
                menu.addAction("&Unset drone");
            }
            else
            {
                menu.addAction("&Set drone");
            }

            if (nodeItem->getNode()->getCategory() == graph::Node::Category::Attacker)
            {
                menu.addAction("&Unset attacker");
            }
            else
            {
                menu.addAction("&Set attacker");
            }

            if (nodeItem->getNode()->getCategory() == graph::Node::Category::Target)
            {
                menu.addAction("&Unset target");
            }
            else
            {
                menu.addAction("&Set target");
            }

            QAction *act = menu.exec(event->globalPos());
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

                if (act->text() == "&Set node corner")
                {
                    emit sigSetNodeType(nodeName, graph::Node::Type::Corner);
                }

                if (act->text() == "&Set node border")
                {
                    emit sigSetNodeType(nodeName, graph::Node::Type::Border);
                }

                if (act->text() == "&Set node inner")
                {
                    emit sigSetNodeType(nodeName, graph::Node::Type::Inner);
                }

                if (act->text() == "&Set drone")
                {
                    std::string ip, port;
                    DroneIpInputDialog oDialog;
                    if (oDialog.exec() == QDialog::Accepted)
                    {
                        ip = oDialog.getIp().toStdString();
                        port = oDialog.getPort().toStdString();
                    }

                    if (ip.empty() || port.empty())
                    {
                        QMessageBox::warning(this, tr("Drone Ip or Port is not set"),
                                             tr("Can't set drone without ip and port."));
                        return;
                    }

                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Drone, ip, port);
                }

                if (act->text() == "&Unset drone")
                {
                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Generic);
                }

                if (act->text() == "&Set attacker")
                {
                    std::string ip, port;
                    DroneIpInputDialog oDialog;
                    if (oDialog.exec() == QDialog::Accepted)
                    {
                        ip = oDialog.getIp().toStdString();
                        port = oDialog.getPort().toStdString();
                    }

                    if (ip.empty() || port.empty())
                    {
                        QMessageBox::warning(this, tr("Drone Ip or Port is not set"),
                                             tr("Can't set drone without ip and port."));
                        return;
                    }

                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Attacker, ip, port);
                }

                if (act->text() == "&Unset attacker")
                {
                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Generic);
                }

                if (act->text() == "&Set target")
                {
                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Target);
                }

                if (act->text() == "&Unset target")
                {
                    emit sigSetNodeCategory(nodeName, graph::Node::Category::Generic);
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
