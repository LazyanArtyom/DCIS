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
#ifndef DCIS_GUI_GRAPHEDITINGTOOL_H_
#define DCIS_GUI_GRAPHEDITINGTOOL_H_

// App includes
#include <gui/graphview.h>
#include <gui/imageeditor.h>

// QT includes
#include <QWidget>

namespace dcis::gui
{

class GraphEditingTool : public QWidget
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

    GraphEditingTool(QWidget *parent = nullptr);

    void showImage(const QImage &img);
    void showNewNodeDialog(QPointF pos = QPointF(0, 0));

    graph::Graph *getGraph() const;
    void updateGraph(graph::Graph *graph);

    void setFocus(bool toImageEditor);
    SizeInfo getSizeInfo() const;

  signals:
    void sigNodeMoved();
    void sigGraphChanged();

  protected:
    void resizeEvent(QResizeEvent *) override;

  private:
    graph::Graph *graph_;
    GraphView *graphView_;
    GraphScene *graphScene_;
    ImageEditor *imageEditor_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHEDITINGTOOL_H_
