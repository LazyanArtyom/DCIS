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
#include <utils/imageprovider.h>

// QT includes
#include <QDir>
#include <QImageReader>

namespace dcis::core::utils
{

QSize ImageProvider::getCurrentImageSize() const
{
    QImageReader imgReader(currentImagePath_);
    if (imgReader.canRead())
    {
        QImage img = imgReader.read();
        return QSize(img.width(), img.height());
    }

    return QSize(0, 0);
}

QString ImageProvider::getCurrentImagePath() const
{
    return currentImagePath_;
}

void ImageProvider::setWorkingDirectoryPath(const QString path)
{
    QDir dir;
    workingDirectoryPath_ = path;
    dir.mkdir(workingDirectoryPath_);
}

void ImageProvider::saveRawImage(const QString path, const QByteArray& data)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
    }

    currentImagePath_ = path;
}

QString ImageProvider::getWorkingDirectoryPath() const
{
    return workingDirectoryPath_;
}

}; // end namespace dcis::utils

