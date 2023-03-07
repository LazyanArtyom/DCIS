#ifndef DCIS_GUI_ELEMENTPROPERTIESTABLE_H_
#define DCIS_GUI_ELEMENTPROPERTIESTABLE_H_

// APP includes
#include <graph/graph.h>

// QT includes
#include <QHeaderView>
#include <QTableWidget>


namespace dcis::gui {
using namespace common;

class ElementPropertiesTable : public QTableWidget 
{
    Q_OBJECT
public:
    explicit ElementPropertiesTable(graph::Graph* graph);
    explicit ElementPropertiesTable(graph::Graph* graph, int sectionSize);

public slots:
    void onUnSelected();
    void onGraphChanged();
    void onSetGraph(graph::Graph* graph);
    void onNodeSelected(const std::string& nodeName);
    void onEdgeSelected(const std::string& uName, const std::string& vName);

private:
    void clearTable();
    graph::Graph* graph_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_ELEMENTPROPERTIESTABLE_H_
