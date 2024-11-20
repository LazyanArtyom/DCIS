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
#ifndef DCIS_GUI_COORDINPUTDIALOG_H_
#define DCIS_GUI_COORDINPUTDIALOG_H_

// Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QDoubleValidator>

namespace dcis::gui
{

class CoordInputDialog : public QDialog
{
  public:
    explicit CoordInputDialog(QWidget *parent = nullptr);
    QString leftTopCoordinate() const;
    QString rightBottomCoordinate() const;

  private:
    QPushButton *okButton_         = nullptr;
    QLineEdit *leftTopLatEdit_     = nullptr;
    QLineEdit *leftTopLonEdit_     = nullptr;
    QLineEdit *rightBottomLatEdit_ = nullptr;
    QLineEdit *rightBottomLonEdit_ = nullptr;

  private slots:
    void accept();
};

} // end namespace dcis::gui
#endif // DCIS_GUI_COORDINPUTDIALOG_H_
