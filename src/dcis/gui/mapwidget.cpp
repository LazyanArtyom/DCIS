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
#include <gui/mapwidget.h>

// APP includes
#include <config/configmanager.h>

// QT includes
#include <QFile>
#include <QVBoxLayout>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace dcis::gui
{

MapWidget::MapWidget(QWidget *parent)
{
    webView_ = new QWebEngineView(this);
    webView_->setUrl(QUrl("qrc:/resources/index.html"));

    webChannel_ = new QWebChannel(this);
    webChannel_->registerObject("mapWidget", this);
    webView_->page()->setWebChannel(webChannel_);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(webView_);
    setLayout(layout);
}

void MapWidget::setImagePath(const QString imagePath)
{
    imagePath_ = imagePath;
}

QString MapWidget::getImagePath() const
{
    return imagePath_;
}

QString MapWidget::getLeftTop() const
{
    return leftTop_;
}

void MapWidget::setLeftTop(const QString leftTop)
{
    leftTop_ = leftTop;
}

void MapWidget::setRightBottom(const QString rightBottom)
{
    rightBottom_ = rightBottom;
}

QString MapWidget::getRightBottom() const
{
    return rightBottom_;
}

void MapWidget::onReceiveImageURL(const QString imageData)
{
    QString uploadedImagesPath = common::config::ConfigManager::getConfig("uploaded_images_path").toString();
    QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString imageName = QString("image_%1.png").arg(timeStamp);
    QString fullPath = QString("%1/%2").arg(uploadedImagesPath).arg(imageName);

    QString base64Data = imageData.split(",").last();

    // Decode the base64 data
    QByteArray imageBytes = QByteArray::fromBase64(base64Data.toUtf8());
    QImage image;
    image.loadFromData(imageBytes, "PNG");

    if (image.save(fullPath))
    {
        qDebug() << "Stitched image saved at:" << fullPath;
        imagePath_ = fullPath;
        emit sigDataReady();
    }
    else
    {
        qDebug() << "Failed to save stitched image.";
    }
}

void MapWidget::onReceiveCoords(const QString coords)
{
    QStringList coordList = coords.split(",", Qt::SkipEmptyParts);
    for (QString &coord : coordList)
    {
        coord = coord.trimmed();
    }

    leftTop_ = QString("%1 %2").arg(coordList[0]).arg(coordList[1]);
    rightBottom_ = QString("%1 %2").arg(coordList[2]).arg(coordList[3]);
}

} // end namespace dcis::gui
