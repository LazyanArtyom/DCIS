#ifndef DCIS_GUI_GRAPHSCENE_H_
#define DCIS_GUI_GRAPHSCENE_H_

// App includes
#include <graph/graph.h>
#include <gui/nodeitem.h>
#include <gui/edgeitem.h>

// Qt includes
#include <QtWidgets>
#include <QGraphicsScene>


namespace dcis::gui {

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphScene() = default;
    explicit GraphScene(graph::Graph* graph);
    ~GraphScene() override;

    graph::Graph* getGraph() const;
    void setGraph(graph::Graph* graph);

    NodeItem* getNodeItem(const std::string& name);
    EdgeItem* getEdgeItem(const std::string& uname, const std::string& vname);

    void clearAll();

public slots:
    void onReload();

signals:
    void sigItemMoved();
    void sigNeedRedraw();
    void sigGraphChanged();

private:
    using QGraphicsScene::clear;

    graph::Graph* graph_ = nullptr;
    std::unique_ptr<QTimer> uniqueTimer_;
    std::unordered_map<std::string, NodeItem*> nodeItems_;
    std::unordered_map<std::pair<std::string, std::string>, EdgeItem*> edgeItems_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_GRAPHSCENE_H_
