#ifndef DCIS_COMMON_UTILS_TERMINALWIDGET_H_
#define DCIS_COMMON_UTILS_TERMINALWIDGET_H_

// Qt includes
#include <QContextMenuEvent>
#include <QEvent>
#include <QMenu>
#include <QTextEdit>
#include <QWidget>

namespace dcis::common::utils
{

class TerminalWidget : public QTextEdit
{
    Q_OBJECT
  public:
    TerminalWidget(QWidget *parent = nullptr);
    void setText(QString text, int fontSize = 0);
    void appendText(QString text, int fontSize = 0);
    void clearText();

  protected:
    void contextMenuEvent(QContextMenuEvent *event);
  private slots:
    void adjustScrollBar();

  private:
    void processColorAndLineEnding(QString &text);
};

} // namespace dcis::common::utils
#endif // DCIS_COMMON_UTILS_TERMINALWIDGET_H_
