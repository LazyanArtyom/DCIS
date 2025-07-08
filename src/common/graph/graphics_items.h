
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
#ifndef SWARM_X_LIB_GRAPH_GRAPHICS_ITEMS_H_
#define SWARM_X_LIB_GRAPH_GRAPHICS_ITEMS_H_

// Standard Library

// Qt Headers
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

// Project Headers
#include <graph/node.h>

namespace dcis::common::graph {

class NodeItemBase : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    explicit NodeItemBase(Node *node, QGraphicsItem* parent = nullptr);
    ~NodeItemBase() override = default;

    enum
    {
        Type = UserType + 5
    };

    auto getRadius() const -> int;
    void setRadius(int radius);

    auto getColor() const -> QColor;
    void setColor(QColor color);

    auto getNode() const -> Node*;
    void setNode(Node *node);

protected:
    auto type() const -> int override;
    auto boundingRect() const -> QRectF override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Node* node_;
    
    int radius_ = 40;
    QColor color_ = Qt::green;
    const int fontSize_ = 10;
    const QString font_ = "Source Code Pro";
};

class EdgeItemBase : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    EdgeItemBase(NodeItemBase *startItem, NodeItemBase *endItem, QGraphicsItem *parent = nullptr);

    auto getColor() const -> QColor;
    void setColor(QColor color);

protected:
    auto shape() const -> QPainterPath override;
    auto boundingRect() const -> QRectF override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPainterPath path_;
    NodeItemBase *endItem_;
    NodeItemBase *startItem_;
    QColor color_ = Qt::red;
};

} // end namespace swrm_x::graph
#endif // SWARM_X_LIB_GRAPH_GRAPHICS_ITEMS_H_
