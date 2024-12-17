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
#ifndef DCIS_GUI_MAPWIDGET_H_
#define DCIS_GUI_MAPWIDGET_H_

// App includes

// Qt includes
#include <QWidget>
#include <QWebChannel>
#include <QWebEngineView>

namespace dcis::gui {

class MapWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit MapWidget(QWidget *parent = nullptr);

    QString getImagePath() const;
    void setImagePath(const QString imagePath);

    QString getLeftTop() const;
    void setLeftTop(const QString leftTop);

    QString getRightBottom() const;
    void setRightBottom(const QString rightBottom);

  signals:
    void sigDataReady();

  public slots:
    void onReceiveImageURL(const QString url);
    void onReceiveCoords(const QString url);

  private:
    QString leftTop_;
    QString rightBottom_;
    QString imagePath_;
    QWebEngineView *webView_;
    QWebChannel *webChannel_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_MAPWIDGET_H_
