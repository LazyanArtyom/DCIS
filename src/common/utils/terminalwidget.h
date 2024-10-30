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
#ifndef DCIS_COMMON_UTILS_TERMINALWIDGET_H_
#define DCIS_COMMON_UTILS_TERMINALWIDGET_H_

// Qt includes
#include <QMenu>
#include <QEvent>
#include <QWidget>
#include <QTextEdit>
#include <QContextMenuEvent>

namespace dcis::common::utils
{

class ILogger 
{
public:
    virtual ~ILogger() = default;
    virtual void clearText() = 0;
    virtual void setText(QString text, int fontSize = 0) = 0;
    virtual void appendText(QString text, int fontSize = 0) = 0;
};

class LoggerCore : public ILogger 
{
public:
    void clearText() override;
    void setText(QString text, int fontSize = 0) override;
    void appendText(QString text, int fontSize = 0) override;
};

class LoggerWidget : public QTextEdit, public LoggerCore
{
    Q_OBJECT
  public:
    LoggerWidget(QWidget *parent = nullptr);
    void setText(QString text, int fontSize = 0) override final;
    void appendText(QString text, int fontSize = 0) override final;
    void clearText() override final;

  protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
  private slots:
    void adjustScrollBar();

  private:
    void processColorAndLineEnding(QString &text);
};

} // namespace dcis::common::utils
#endif // DCIS_COMMON_UTILS_TERMINALWIDGET_H_
