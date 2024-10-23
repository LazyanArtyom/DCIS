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
