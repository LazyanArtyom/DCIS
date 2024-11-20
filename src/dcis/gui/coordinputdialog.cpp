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
#include <gui/coordinputdialog.h>

// QT includes
#include <QMessageBox>

namespace dcis::gui
{

CoordInputDialog::CoordInputDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("GeoCoordinate Dialog");

    QDoubleValidator *validator = new QDoubleValidator(0.0, 360.0, 8);
    leftTopLatEdit_ = new QLineEdit(this);
    leftTopLatEdit_->setValidator(validator);
    leftTopLonEdit_ = new QLineEdit(this);
    leftTopLonEdit_->setValidator(validator);
    rightBottomLatEdit_ = new QLineEdit(this);
    rightBottomLatEdit_->setValidator(validator);
    rightBottomLonEdit_ = new QLineEdit(this);
    rightBottomLonEdit_->setValidator(validator);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Left Top Latitude:", leftTopLatEdit_);
    formLayout->addRow("Left Top Longitude:", leftTopLonEdit_);
    formLayout->addRow("Right Bottom Latitude:", rightBottomLatEdit_);
    formLayout->addRow("Right Bottom Longitude:", rightBottomLonEdit_);

    okButton_ = new QPushButton("OK", this);
    okButton_->setDisabled(true);

    auto slot = [&]() {
        okButton_->setEnabled(!leftTopLatEdit_->text().isEmpty() && !leftTopLonEdit_->text().isEmpty() &&
                                !rightBottomLatEdit_->text().isEmpty() && !rightBottomLonEdit_->text().isEmpty());
    };

    connect(leftTopLatEdit_, &QLineEdit::textEdited, this, slot);
    connect(leftTopLonEdit_, &QLineEdit::textEdited, this, slot);
    connect(rightBottomLatEdit_, &QLineEdit::textEdited, this, slot);
    connect(rightBottomLonEdit_, &QLineEdit::textEdited, this, slot);

    connect(okButton_, &QPushButton::clicked, this, &CoordInputDialog::accept);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(okButton_);
}

QString CoordInputDialog::leftTopCoordinate() const
{
    QStringList leftTopList;
    leftTopList << leftTopLatEdit_->text();
    leftTopList << leftTopLonEdit_->text();
    return leftTopList.join(" ");
}

QString CoordInputDialog::rightBottomCoordinate() const
{
    QStringList rightBottomList;
    rightBottomList << rightBottomLatEdit_->text();
    rightBottomList << rightBottomLonEdit_->text();
    return rightBottomList.join(" ");
}

void CoordInputDialog::accept()
{
    // Perform any additional validation or checks here if needed
    QDialog::accept();
}

} // end namespace dcis::gui
