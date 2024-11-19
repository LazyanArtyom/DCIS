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
#ifndef DCIS_GUI_NODEITEM_H_
#define DCIS_GUI_NODEITEM_H_

// App includes
#include <graph/node.h>

// Qt includes
#include <QGraphicsItem>
#include <QPainterPath>

namespace dcis::gui
{
using namespace common;
class GraphScene;

class NodeItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
  public:
    NodeItem(GraphScene *gscene, graph::Node *node);

    enum
    {
        Type = UserType + 5
    };

    int getDefaultRadius();
    QColor getDefaultColor();
    QColor getDefaultSelectedColor();

    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    graph::Node *getNode() const;
    void setNode(graph::Node *node);

    int getRadius() const;

    QColor getColor() const;
    void setColor(const QColor &newColor);

    QColor getSelectedColor() const;
    void setSelectedColor(const QColor &newColor);

  protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  signals:
    void sigPositionChanged();

  private:
    int radius_;
    bool isMoving_;

    graph::Node *node_ = nullptr;
    GraphScene *gscene_ = nullptr;

    const int fontSize_ = 10;
    const QString font_ = "Source Code Pro";
};

} // end namespace dcis::gui
#endif // DCIS_GUI_NODEITEM_H_
