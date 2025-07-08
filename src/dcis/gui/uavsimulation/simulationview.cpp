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
#include <gui/uavsimulation/simulationview.h>

// APP includes

// STL includes
#include <iostream>
#include <sstream>
#include <string>

namespace dcis::gui
{

UAVSimulationView::UAVSimulationView(QWidget *parent) : QGraphicsView(parent)
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

    graphScene_ = new UAVSimulationScene();
    setScene(graphScene_);
}

void UAVSimulationView::setGraph(common::graph::Graph *graph)
{
    graphScene_->updateGraph(graph);
}

void UAVSimulationView::setScene(UAVSimulationScene *scene)
{
    graphScene_ = scene;
    QGraphicsView::setScene(scene);
}

void UAVSimulationView::setSceneSize(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

void UAVSimulationView::test()
{
    graphScene_->testAnimation();
}

void UAVSimulationView::viewFit()
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    isResized_ = true;

    if (sceneRect().width() > sceneRect().height())
        isLandscape_ = true;
    else
        isLandscape_ = false;
}

void UAVSimulationView::scaleView(qreal scaleFactor)
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

UAVSimulationView::ImageInfo UAVSimulationView::getImageInfo()
{
    imageInfo_.sceneRectSize = QSizeF(scene()->sceneRect().width(), scene()->sceneRect().height());
    imageInfo_.viewportSize = viewport()->size();

    return imageInfo_;
}

auto UAVSimulationView::getImage() const -> QImage           
{                                              
    if (pixmapItem_) {                         
        return pixmapItem_->pixmap().toImage();
    }                                          
    return QImage();                           
}                                              

void UAVSimulationView::setImage(const QImage &img)
{
    if (!scene())
        return;

    if (!scene()->sceneRect().isEmpty())
        scene()->clear();

    imageInfo_.imageSize = QSizeF(img.width(), img.height());
    pixmapItem_ = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    scene()->addItem(pixmapItem_);

    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    pixmapItem_->setPos(scene()->width() / 2 - pixmapItem_->boundingRect().width() / 2,
                        scene()->height() / 2 - pixmapItem_->boundingRect().height() / 2);
}

void UAVSimulationView::resizeEvent(QResizeEvent *event)
{
    isResized_ = true;
    QGraphicsView::resizeEvent(event);
}

} // end namespace dcis::gui