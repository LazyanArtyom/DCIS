#include <utils/terminalwidget.h>

// Qt includes

//Color codes
#define T_CC_BLACK           "\033[30m"
#define T_BOLD_CC_RED        "\033[1;31m"
#define T_BOLD_CC_GREEN      "\033[1;32m"
#define T_BOLD_CC_BLUE       "\033[1;34m"
#define T_BOLD_CC_YELLOW     "\033[1;33m"
#define T_BOLD_CC_GREY       "\033[1;37m"
#define T_BOLD_CC_PING       "\033[1;35m"

#define BG_CC_GREY           "\033[47m"
#define BG_BOLD_CC_RED       "\033[1;41m"
#define BG_BOLD_CC_GREEN     "\033[1;42m"
#define BG_BOLD_CC_YELLOW    "\033[1;43m"
#define BG_BOLD_CC_PINK      "\033[1;45m"
#define BG_BOLD_CC_BLUE      "\033[1;44m"
#define BG_BOLD_CC_GREY      "\033[1;47m"

#define T_TO_BOLD            "\033[1m"
#define CC_NON               "\033[m\017"
#define BOLD_NON             "\033[0m"

#define TXT_TO_RED(__msg)    (T_BOLD_CC_RED __msg CC_NON)
#define TXT_TO_GREEN(__msg)  (T_BOLD_CC_GREEN __msg CC_NON)
#define TXT_TO_BLUE(__msg)   (T_BOLD_CC_BLUE __msg CC_NON)
#define TXT_TO_YELLOW(__msg) (T_BOLD_CC_YELLOW __msg CC_NON)
#define TXT_TO_PING(__msg)   (T_BOLD_CC_PING __msg CC_NON)
#define TXT_TO_BOLD(__msg)    (T_TO_BOLD __msg BOLD_NON)


#define BG_TO_RED(__msg)    (BG_BOLD_CC_RED __msg CC_NON)
#define BG_TO_GREEN(__msg)  (BG_BOLD_CC_GREEN __msg CC_NON)
#define BG_TO_YELLOW(__msg) (BG_BOLD_CC_YELLOW __msg CC_NON)
#define BG_TO_PINK(__msg)   (BG_BOLD_CC_PINK __msg CC_NON)
#define BG_TO_BLUE(__msg)   (BG_BOLD_CC_BLUE __msg CC_NON)


namespace dcis::common::utils {

void TerminalWidget::processColorAndLineEnding(QString& text)
{
    text.replace(" ", "&nbsp;");
    text.replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
    text.replace("<", "&lt;"); // replace < with &lt;
    text.replace(">", "&gt;"); // replace > with &gt;
    text.replace(T_BOLD_CC_RED, "<b style='color:red'>");
    text.replace(T_BOLD_CC_GREEN, "<b style='color:green'>");
    text.replace(T_BOLD_CC_YELLOW, "<b style='color:yellow'>");
    text.replace(T_BOLD_CC_BLUE, "<b style='color:blue'>");
    text.replace(T_BOLD_CC_GREY, "<b style='color:grey'>");
    text.replace(T_CC_BLACK, "");

    text.replace(BG_BOLD_CC_RED, "<span style='background-color:red'>");
    text.replace(BG_BOLD_CC_GREEN, "<span style='background-color:green'>");
    text.replace(BG_BOLD_CC_YELLOW, "<span style='background-color:yellow'>");
    text.replace(BG_BOLD_CC_PINK, "<span style='background-color:pink'>");
    text.replace(BG_BOLD_CC_BLUE, "<span style='background-color:blue'>");
    text.replace(BG_BOLD_CC_GREY, "");
    text.replace(BG_BOLD_CC_GREY, "");

    text.replace(CC_NON, "</b>");
    text.replace(T_TO_BOLD, "<b>");
    text.replace(BOLD_NON, "</b>");
    text.replace("\n", "<br>");
}

TerminalWidget::TerminalWidget(QWidget* parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    setStyleSheet("background-color: black; color: white");
    setFont(QFont("Courier New", 12));

    connect(this, &QTextEdit::textChanged, this, &TerminalWidget::adjustScrollBar);
}

void TerminalWidget::setText(QString text, int fontSize)
{
    if(fontSize)
    {
        setFont(QFont("Courier New", fontSize));
    }

    processColorAndLineEnding(text);
    setText("<pre>" + text + "</pre>");
}

void TerminalWidget::appendText(QString text, int fontSize)
{
    processColorAndLineEnding(text);
    insertHtml("<pre>" + text + "</pre>");
}

void TerminalWidget::clearText()
{
    clear();
}

void TerminalWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QFont font;
    font.setPointSize(14); // Set the font size to 14

    menu->setFont(font);
    menu->exec(event->globalPos());
    delete menu;
}

void TerminalWidget::adjustScrollBar()
{
    moveCursor(QTextCursor::End);
    ensureCursorVisible();
}

} // end namespace vta::common::utils
