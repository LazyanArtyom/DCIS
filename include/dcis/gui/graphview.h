#ifndef DCIS_GUI_GRAPHVIEW_H_
#define DCIS_GUI_GRAPHVIEW_H_

// App includes
#include <gui/graphscene.h>

// Qt includes
#include <QMenu>
#include <QGraphicsView>
#include <QContextMenuEvent>


namespace dcis::gui {

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphView();

    void setScene(GraphScene* scene);

public slots:
    void onRedraw();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void sigNodeEdited(std::string name);
    void sigNodeRemoved(std::string name);
    void sigNodeIsolated(std::string name);
    void sigNodeSelected(std::string name);
    void sigNodeAdded(QPointF pos, bool autoNaming);

    void sigEdgeAdded();
    void sigEdgeAddedFrom(std::string name);
    void sigEdgeSet(std::string uname, std::string vname);
    void sigEdgeRemoved(std::string uname, std::string vname);
    void sigEdgeSelected(std::string uname, std::string vname);

    void sigUnSelected();
    void sigNeedRefresh();

private:
    bool isMoving_ = false;
    bool isSelectTargetNode = false;

    qreal currentScale_ = 1.;
    const qreal scaleMax_ = 1.5;

    QMenu* contextMenu_;
    GraphScene* gcene_;
    NodeItem* startItem_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHVIEW_H_
