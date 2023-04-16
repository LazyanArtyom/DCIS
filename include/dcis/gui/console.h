#ifndef DCIS_GUI_CONSOLE_H_
#define DCIS_GUI_CONSOLE_H_

// App includes
#include <iostream>

// Qt includes
#include <QTextEdit>


namespace dcis::gui {

class Console : public QTextEdit
{
public:
    Console(QWidget* parent = nullptr);
};

} // end namespace dcis::gui
#endif // DCIS_GUI_CONSOLE_H_
