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
#ifndef DCIS_GUI_UAVSIMULATION_SIMULATIONVIEW_H_
#define DCIS_GUI_UAVSIMULATION_SIMULATIONVIEW_H_

// App includes
#include <gui/uavsimulation/simulationscene.h>

// Qt includes
#include <QMenu>
#include <QGraphicsView>
#include <QContextMenuEvent>

namespace dcis::gui
{

class UAVSimulationView : public QGraphicsView
{
    Q_OBJECT
public:
    struct ImageInfo
    {
        QSizeF imageSize;
        QSizeF viewportSize;
        QSizeF sceneRectSize;
    };
    
    UAVSimulationView(QWidget *parent = nullptr);

    void setGraph(common::graph::Graph *graph);
    void setScene(UAVSimulationScene *scene);
    void setSceneSize(int width, int height);
    void test();

    void viewFit();
    void scaleView(qreal scaleFactor);

    ImageInfo getImageInfo();
    auto getImage() const -> QImage;
    void setImage(const QImage &img);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    qreal currentScale_ = 1.;
    const qreal scaleMax_ = 1.5;

    bool isResized_ = false;
    bool isLandscape_ = false;
    ImageInfo imageInfo_;
    QPixmap backgroundImage_;
    common::graph::Graph *graph_;
    UAVSimulationScene *graphScene_;

    QGraphicsPixmapItem *pixmapItem_ = nullptr;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_UAVSIMULATION_SIMULATIONVIEW_H_
