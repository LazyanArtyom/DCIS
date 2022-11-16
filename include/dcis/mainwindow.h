#ifndef DCIS_UI_MAINWINDOW_H_
#define DCIS_UI_MAINWINDOW_H_

// App headers
#include <net/client.h>
#include <utils/debugstream.h>

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
#include <QTabWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>

namespace dcis::ui {
using namespace common;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupUi();

public slots:
    void onBtnSendMsgClicked();
    void onMsgReieved(const QString msg);

private:
    client::Client* client_;
    std::unique_ptr<common::utils::DebugStream> debugStream_;

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

    QPushButton *BFSbtn_;
    QPushButton *DFSbtn_;

    QWidget *utilsWidget_;
    QWidget *workingWidget_;

    // central tab
    QTabWidget *centralTabWidget_;

    // Entry widget start
    QWidget *entryWidget_;
    QLineEdit   *ip_;
    QLineEdit   *port_;
    QPushButton *btnConnect_;
    // Entry widget end

    // Working widget start
    QTextEdit *txtConsole_;
    QLineEdit *txtMessage_;
    QPushButton *btnMsgSend_;
    // Working widget end

    QWidget *tabAdjMat_;
    QWidget *tabIncMat_;
    QWidget *tabVisualization_;

    QFrame *line_;
    QFrame *line_4_;

    QLabel *label_;
    QLabel *algorithmsLabel_;

    QStatusBar *statusBar_;
    QTabWidget *tabWidget_;

    QSlider *horizontalSlider_;
    QSplitter  *verticalSplitter_;

    QHBoxLayout *horizontalLayoutCentral_;
    QHBoxLayout *horizontalLayout_3_;
    QHBoxLayout *horizontalLayout_2_;
    QVBoxLayout *visualLayout_;
    QVBoxLayout *verticalLayout_3_;
    QVBoxLayout *adjMatLayout_;
    QVBoxLayout *verticalLayout_5_;
    QVBoxLayout *incMatLayout_;
    QVBoxLayout *verticalLayout_;
    QVBoxLayout *propertiesLayout_;
    QVBoxLayout *verticalLayout_4_;
    QVBoxLayout *verticalLayout_6_;
    QHBoxLayout *horizontalLayout_4_;
    QHBoxLayout *horizontalLayout_5_;
    QHBoxLayout *horizontalLayout_6_;
};

} // end namespace dcis::ui
#endif // DCIS_UI_MAINWINDOW_H_
