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
    GraphView(QWidget* parent = nullptr);

    void setScene(GraphScene* scene);
    void setSceneSize(int width, int height);

public slots:
    void onRedraw();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

signals:
    void sigNodeEdited(std::string name);
    void sigNodeRemoved(std::string name);
    void sigNodeIsolated(std::string name);
    void sigNodeAdded(QPointF pos, bool autoNaming);
    void sigNodeSelected(std::string name, QPointF pos);
    void sigNodeMoved();

    void sigEdgeAdded();
    void sigEdgeAddedFrom(std::string name);
    void sigEdgeSet(std::string uname, std::string vname);
    void sigEdgeRemoved(std::string uname, std::string vname);
    void sigEdgeSelected(std::string uname, std::string vname);

    void sigUnSelected();

private:
    bool isMoving_ = false;
    bool isSelectTargetNode_ = false;

    QMenu* contextMenu_;
    GraphScene* gscene_;
    NodeItem* startItem_;

    qreal currentScale_ = 1.;
    const qreal scaleMax_ = 1.5;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHVIEW_H_
