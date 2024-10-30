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
#ifndef DCIS_GUI_IMAGEEDITOR_H_
#define DCIS_GUI_IMAGEEDITOR_H_

// App includes
#include <gui/graphscene.h>

// Qt includes
#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

namespace dcis::gui
{

class ImageEditor : public QGraphicsView
{
    Q_OBJECT
  public:
    struct SizeInfo
    {
        QSizeF imageSize;
        QSizeF imageSizeZoomed;
        QSizeF imageViewportSize;
        QSizeF graphViewportSize;
    };

    ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor() = default;

    void zoomIn();
    void zoomOut();
    void viewFit();

    SizeInfo getSizeInfo() const;
    void setImage(const QImage &img);
    void showNewNodeDialog(QPointF pos = QPointF(0, 0));

    graph::Graph *getGraph() const;
    void updateGraph(graph::Graph *graph);

  signals:
    void sigNodeMoved();
    void sigGraphChanged();

  public slots:

  protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

  private:
    void scaleView(qreal scaleFactor);
    bool isResized_ = false;
    bool isLandscape_ = false;
    QPixmap backgroundImage_;

    graph::Graph *graph_;
    GraphScene *graphScene_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_IMAGEEDITOR_H_
