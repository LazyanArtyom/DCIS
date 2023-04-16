#include <gui/console.h>


namespace dcis::gui {

Console::Console(QWidget* parent)
    : QTextEdit(parent)
{
    setObjectName("consoleText");
    QSizePolicy sizePolicyConsole(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicyConsole.setHorizontalStretch(240);
    sizePolicyConsole.setVerticalStretch(0);
    sizePolicyConsole.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicyConsole);
    setMinimumSize(QSize(0, 240));
    setMaximumSize(QSize(16777215, 720));
    setReadOnly(true);
}

} // end namespace dcis::gui
