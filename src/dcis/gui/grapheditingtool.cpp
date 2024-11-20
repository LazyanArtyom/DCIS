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
#include <gui/grapheditingtool.h>

namespace dcis::gui
{

GraphEditingTool::GraphEditingTool(QWidget *parent) : QWidget(parent)
{
    // image editor widget
    imageEditor_ = new ImageEditor();

    // graph view widget
    graph_ = new graph::Graph(false);
    graphScene_ = new GraphScene(graph_);

    graphView_ = new GraphView(imageEditor_);
    graphView_->setScene(graphScene_);

    // connections
    connect(this, &GraphEditingTool::sigGraphChanged, graphScene_, &GraphScene::onReload);

    connect(graphView_, &GraphView::sigNodeSelected, this, [this](const std::string &nodeName, QPointF pos) {
        // txtConsole_->setText(txt);
    });

    connect(graphView_, &GraphView::sigNodeMoved, this, [this]() { 
        emit sigNodeMoved(); 
    });

    connect(graphView_, &GraphView::sigNodeAdded, this, [this](QPointF pos, bool autoNaming) {
        if (!autoNaming)
        {
            showNewNodeDialog(pos);
            return;
        }

        graph_->addNode(graph::Node(graph_->getNextNodeName(), pos));
        emit sigGraphChanged();
    });

    connect(graphView_, &GraphView::sigNodeRemoved, this, [this](const std::string &nodeName) {
        if (graph_->removeNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });

    connect(graphView_, &GraphView::sigNodeIsolated, this, [this](const std::string &nodeName) {
        if (graph_->isolateNode(nodeName))
        {
            emit sigGraphChanged();
        }
    });

    connect(graphView_, &GraphView::sigEdgeRemoved, this, [this](const std::string &uname, const std::string &vname) {
        if (graph_->removeEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });

    connect(graphView_, &GraphView::sigEdgeSet, this, [this](const std::string &uname, const std::string &vname) {
        if (graph_->setEdge(uname, vname))
        {
            emit sigGraphChanged();
        }
    });

    connect(graphView_, &GraphView::sigNodeEdited, this, [this](const std::string &nodeName) {
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

    connect(graphView_, &GraphView::sigSetNodeCategory, this, 
            [this](std::string nodeName, graph::Node::Category nodeCategory) {
        auto node = graph_->getNode(nodeName);
        if (node)
        {
            node->setCategory(nodeCategory);
            emit sigGraphChanged();
        }
    });

    connect(graphView_, &GraphView::sigSetNodeType, this,
            [this](const std::string &nodeName, const graph::Node::Type &nodeType) {
        auto node = graph_->getNode(nodeName);
        if (node)
        {
            node->setType(nodeType);
            emit sigGraphChanged();
        }
    });

    // layouts
    QVBoxLayout *vMainLayout = new QVBoxLayout();
    vMainLayout->setContentsMargins(0, 0, 0, 0);
    vMainLayout->setSpacing(0);

    setLayout(vMainLayout);
    vMainLayout->addWidget(imageEditor_);
}

void GraphEditingTool::showNewNodeDialog(QPointF pos)
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

graph::Graph *GraphEditingTool::getGraph() const
{
    return graph_;
}

void GraphEditingTool::updateGraph(graph::Graph *graph)
{
    graph_ = graph;
    graphScene_->setGraph(graph);
}

void GraphEditingTool::setFocus(bool toImageEditor)
{
    if (toImageEditor)
    {
        graphView_->setAutoFillBackground(false);

        // set the attribute to be transparent for mouse events
        graphView_->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    else
    {
        graphView_->setAutoFillBackground(true);

        // set the attribute to be transparent for mouse events
        graphView_->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
}

GraphEditingTool::SizeInfo GraphEditingTool::getSizeInfo() const
{
    SizeInfo info;

    // Get the current transformation matrix
    QTransform transform = imageEditor_->transform();

    // Get the size of the image in scene coordinates
    QRectF imageRect = imageEditor_->sceneRect();
    info.imageSize = imageRect.size();

    // Apply the transformation matrix to the image rectangle
    QRectF transformedRect = transform.mapRect(imageRect);

    // Get the size of the transformed rectangle
    info.imageSizeZoomed = transformedRect.size();

    info.imageViewportSize = imageEditor_->size();
    info.graphViewportSize = graphView_->size();

    return info;
}

void GraphEditingTool::showImage(const QImage &img)
{
    imageEditor_->setImage(img);
}

void GraphEditingTool::resizeEvent(QResizeEvent *)
{
    imageEditor_->resize(width(), height());
    graphView_->resize(width(), height());
    // graphView_->setSceneRect(graphView_->rect());
}

} // end namespace dcis::gui
