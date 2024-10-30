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
#ifndef DCIS_GUI_EDGEITEM_H_
#define DCIS_GUI_EDGEITEM_H_

// App includes
#include <graph/edge.h>
#include <gui/nodeitem.h>

// Qt includes
class QRectF;
class QPainterPath;
class QGraphicsScene;
class QGraphicsLineItem;
class GraphGraphicsScene;
class QGraphicsSceneMouseEvent;

namespace dcis::gui
{
using namespace common;

class GraphScene;
class EdgeItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
  public:
    enum
    {
        Type = UserType + 4
    };

    EdgeItem(GraphScene *scene, NodeItem *startItem, NodeItem *endItem, QGraphicsItem *parent = nullptr);

    static QColor getDefaultColor();
    static QColor getDefaultSelectedColor();

    int type() const override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;

    graph::Edge getEdge() const;
    bool isInversionAvailable() const;

  public slots:
    void onUpdatePosition();

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  private:
    QPainterPath path_;
    NodeItem *endItem_;
    GraphScene *gscene_;
    NodeItem *startItem_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_EDGEITEM_H_
