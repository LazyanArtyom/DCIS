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
    setCacheMode(CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

auto UAVSimulationView::getImage() const -> QImage           
{                                              
    if (pixmapItem_ != nullptr) {                         
        return pixmapItem_->pixmap().toImage();
    }                                          
    return QImage{};                           
}                                              

void UAVSimulationView::setImage(const QImage &img)
{
    if (!scene()->sceneRect().isEmpty())
    {
        scene()->clear();
    }

    pixmapItem_ = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    scene()->addItem(pixmapItem_);

    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    pixmapItem_->setPos((scene()->width() / 2) - (pixmapItem_->boundingRect().width() / 2),
                        (scene()->height() / 2) - (pixmapItem_->boundingRect().height() / 2));
}

} // end namespace dcis::gui