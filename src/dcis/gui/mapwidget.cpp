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
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace dcis::gui
{

MapWidget::MapWidget(QWidget *parent)
{
    webView_ = new QWebEngineView(this);
    webView_->setContextMenuPolicy(Qt::NoContextMenu);
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
    if (coordList.size() < 4)
    {
        qWarning() << "Invalid map coordinates received:" << coords;
        return;
    }

    for (QString &coord : coordList)
    {
        coord = coord.trimmed();
    }

    leftTop_ = QString("%1 %2").arg(coordList[0]).arg(coordList[1]);
    rightBottom_ = QString("%1 %2").arg(coordList[2]).arg(coordList[3]);
}

void MapWidget::onAreaSelected(const QString coords, double x, double y, double width, double height)
{
    onReceiveCoords(coords);

    QRect selectionRect(qRound(x), qRound(y), qRound(width), qRound(height));
    selectionRect = selectionRect.normalized().intersected(webView_->rect());

    if (!selectionRect.isValid() || selectionRect.width() < 2 || selectionRect.height() < 2)
    {
        qWarning() << "Invalid map selection rectangle:" << selectionRect;
        return;
    }

    auto saveScreenshot = [this](const QRect &rect) -> bool {
        QString uploadedImagesPath = common::config::ConfigManager::getConfig("uploaded_images_path").toString();
        QDir().mkpath(uploadedImagesPath);

        QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QString imageName = QString("image_%1.png").arg(timeStamp);
        QString fullPath = QDir(uploadedImagesPath).filePath(imageName);

        QPixmap screenshot = webView_->grab(rect);
        if (!screenshot.isNull() && screenshot.save(fullPath, "PNG"))
        {
            qDebug() << "Map selection saved at:" << fullPath << "size:" << screenshot.size();
            imagePath_ = fullPath;
            return true;
        }

        qWarning() << "Failed to save map selection.";
        return false;
    };

    QString escapedCoords = coords;
    escapedCoords.replace("\\", "\\\\");
    escapedCoords.replace("'", "\\'");

    QString prepareScript = QString("prepareHighResolutionCapture('%1');").arg(escapedCoords);
    webView_->page()->runJavaScript(prepareScript, [this, selectionRect, saveScreenshot](const QVariant &prepared) {
        if (!prepared.toBool())
        {
            QTimer::singleShot(100, this, [this, selectionRect, saveScreenshot]() {
                if (saveScreenshot(selectionRect))
                {
                    emit sigDataReady();
                }
            });
            return;
        }

        QTimer::singleShot(1800, this, [this, selectionRect, saveScreenshot]() {
            webView_->page()->runJavaScript("getHighResolutionCaptureRect();",
                                           [this, selectionRect, saveScreenshot](const QVariant &value) {
                                               QRect captureRect;
                                               QStringList parts = value.toString().split(",", Qt::SkipEmptyParts);

                                               if (parts.size() == 4)
                                               {
                                                   bool okX = false;
                                                   bool okY = false;
                                                   bool okWidth = false;
                                                   bool okHeight = false;

                                                   QRect candidate(qRound(parts[0].toDouble(&okX)),
                                                                   qRound(parts[1].toDouble(&okY)),
                                                                   qRound(parts[2].toDouble(&okWidth)),
                                                                   qRound(parts[3].toDouble(&okHeight)));
                                                   candidate = candidate.normalized().intersected(webView_->rect());

                                                   if (okX && okY && okWidth && okHeight && candidate.width() >= 2 &&
                                                       candidate.height() >= 2)
                                                   {
                                                       captureRect = candidate;
                                                   }
                                               }

                                               if (!captureRect.isValid())
                                               {
                                                   qWarning() << "High resolution capture rect is invalid, using "
                                                                 "visible selection fallback.";
                                                   webView_->page()->runJavaScript("restoreMapAfterHighResolutionCapture();");
                                                   QTimer::singleShot(300, this, [this, selectionRect, saveScreenshot]() {
                                                       if (saveScreenshot(selectionRect))
                                                       {
                                                           emit sigDataReady();
                                                       }
                                                   });
                                                   return;
                                               }

                                               bool saved = saveScreenshot(captureRect);
                                               webView_->page()->runJavaScript("restoreMapAfterHighResolutionCapture();");
                                               if (saved)
                                               {
                                                   emit sigDataReady();
                                               }
                                           });
        });
    });
}

} // end namespace dcis::gui
