#ifndef DCIS_GUI_GRAPHEDITINGTOOL_H_
#define DCIS_GUI_GRAPHEDITINGTOOL_H_

// App includes
#include <gui/graphview.h>
#include <gui/imageeditor.h>

// QT includes
#include <QWidget>


namespace dcis::gui {

class GraphEditingTool : public QWidget
{
    Q_OBJECT
public:
    GraphEditingTool(QWidget* parent = nullptr);

    void showNewNodeDialog(QPointF pos = QPointF(0, 0));

public slots:
    void onUploadImage();

signals:
    void sigGraphChanged();

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    graph::Graph* graph_;
    gui::GraphView* graphView_;
    gui::GraphScene* graphScene_;
    gui::ImageEditor* imageEditor_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHEDITINGTOOL_H_

