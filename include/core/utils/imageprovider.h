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
#ifndef DCIS_CORE_UTILS_IMAGEPROVIDER_H_
#define DCIS_CORE_UTILS_IMAGEPROVIDER_H_

// App includes

// QT includes
#include <QSize>

namespace dcis::core::utils
{

class ImageProvider
{
public:
    QSize getCurrentImageSize() const;
    QString getCurrentImagePath() const;

    QString getWorkingDirectoryPath() const;
    void setWorkingDirectoryPath(const QString path);

    void saveRawImage(const QString path, const QByteArray& data);

private:
    QString currentImageSize_;
    QString currentImagePath_;
    QString workingDirectoryPath_;
};

} // end namespace dcis::core::utils
#endif // DCIS_CORE_UTILS_IMAGEPROVIDER_H_
