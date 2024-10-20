#ifndef DCIS_UI_MAINWINDOW_H_
#define DCIS_UI_MAINWINDOW_H_

// App headers
#include <gui/elementpropertiestable.h>
#include <gui/graphinfo.h>
#include <gui/graphview.h>
#include <net/client.h>
#include <utils/terminalwidget.h>

// QT headers
#include <QAction>
#include <QApplication>
#include <QFrame>
#include <QGeoCoordinate>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

namespace dcis::gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupUi();
    void setWorkspaceEnabled(bool enable);

  protected:
    void resizeEvent(QResizeEvent *event) override;

  public slots:
    void onUpload();
    void onCreateGrid();
    void onClearCycles();
    void onGenerateGraph();
    void onStartExploration();
    void onStartAttack();
    void onConnectBtnClicked();
    void onGraphChanged();
    void onUpdateGraph(const QJsonDocument &json);

    void onSaveGraph();
    void onLoadGraph();

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
    QToolBar *toolBar_;

    // Workspace
    QWidget *workingWidget_;
    QDockWidget *dockWidget_;
    QStackedWidget *centralWidget_;
    QWidget *entryWidget_ = nullptr;
    common::utils::ILogger *terminalWidget_;

    // Entry Widget
    QLabel *backgroundLabel_;
    QLineEdit *username_;
    QLineEdit *password_;
    QLineEdit *ipLineEdit_;
    QLineEdit *portLineEdit_;
    QPushButton *connectButton_;

    client::Client *client_;
    GraphInfo *graphInfo_;
    GraphView *graphView_;

    QString currentFilePath_ = "";

    QString leftTop_;
    QString rightBottom_;
    ElementPropertiesTable *elementPropertiesTable_;
};

} // namespace dcis::gui
#endif // DCIS_UI_MAINWINDOW_H_
