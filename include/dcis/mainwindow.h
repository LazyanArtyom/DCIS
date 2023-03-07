#ifndef DCIS_UI_MAINWINDOW_H_
#define DCIS_UI_MAINWINDOW_H_

// App headers
#include <net/client.h>
#include <gui/graphinfo.h>
#include <utils/debugstream.h>
#include <gui/grapheditingtool.h>
#include <gui/elementpropertiestable.h>

// QT headers
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QWidget>
#include <QSlider>
#include <QAction>
#include <QVariant>
#include <QMenuBar>
#include <QLineEdit>
#include <QSplitter>
#include <QTextEdit>
#include <QStatusBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QStackedWidget>

namespace dcis::ui {
using namespace common;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setupUi();
    void setWorkspaceEnabled(bool enable);

public slots:
    void onConnectBtnClicked();

private:
    void createMenu();
    void createToolBar();
    void createEntryWidget();
    void createWorkingWiget();
    void createGraphInfoWidget();

private:
    QAction *actSave_;
    QAction *actExit_;
    QAction *actSaveAs_;
    QAction *actCredits_;
    QAction *actNewGraph_;
    QAction *actOpenGraph_;

    QAction *actionBFS_;
    QAction *actionDFS_;
    QAction *actAddNode_;
    QAction *actAddEdge_;
    QAction *actDelNode_;
    QAction *actDelEdge_;
    QAction *actEditEdge_;

    QMenu *menuFile_;
    QMenu *menuGraph_;
    QMenu *menuAdd_;
    QMenu *menuEdit_;
    QMenu *menuDelete_;
    QMenu *menuAlgorithms_;

    QMenuBar *menuBar_;

    QStackedWidget *centralWidget_;

    QWidget *entryWidget_;
    QLineEdit   *ip_;
    QLineEdit   *port_;
    QPushButton *btnConnect_;

    QWidget *workingWidget_;
    QTextEdit *txtConsole_;

    client::Client* client_;
    gui::GraphInfo* graphInfo_;
    gui::GraphEditingTool* graphEditingTool_;

    gui::ElementPropertiesTable* elementPropertiesTable_;
   // std::unique_ptr<common::utils::DebugStream> debugStream_;
};

} // end namespace dcis::ui
#endif // DCIS_UI_MAINWINDOW_H_
