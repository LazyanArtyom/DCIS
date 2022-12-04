#include <gui/graphscene.h>


namespace dcis::gui {

GraphScene::GraphScene(graph::Graph* graph)
{
    setGraph(graph);
}

GraphScene::~GraphScene()
{
    clearAll();
}

graph::Graph* GraphScene::getGraph() const
{
    return graph_;
}

void GraphScene::setGraph(graph::Graph* graph)
{
    graph_ = graph;
    onReload();
}

NodeItem* GraphScene::getNodeItem(const std::string& name)
{
    if (nodeItems_.find(name) != nodeItems_.end())
    {
        return nodeItems_[name];
    }

    return nullptr;
}

EdgeItem* GraphScene::getEdgeItem(const std::string& uname, const std::string& vname)
{
    if (edgeItems_.find(std::make_pair(uname, vname)) != edgeItems_.end())
    {
        return edgeItems_.find(std::make_pair(uname, vname))->second;
    }
    else if (!graph_->isDirected() && edgeItems_.find(std::make_pair(vname, uname)) != edgeItems_.end())
    {
        return edgeItems_.find(std::make_pair(vname, uname))->second;
    }

    return nullptr;
}

void GraphScene::clearAll()
{
    clear();
    nodeItems_.clear();
    edgeItems_.clear();
}

void GraphScene::onReload()
{
    clearAll();
    for (const auto& node : getGraph()->getNodes())
    {
        nodeItems_[node->getName()] = new NodeItem(this, node);
    }

    for (auto it = getGraph()->getEdges().begin(); it != getGraph()->getEdges().end(); ++it)
    {
        auto edge = graph::Edge(it);
        auto edgeItem = new EdgeItem(this, nodeItems_[edge.u()->getName()], nodeItems_[edge.v()->getName()]);
        edgeItems_.insert({std::make_pair(edge.u()->getName(), edge.v()->getName()), edgeItem});
    }

    for (const auto& item: edgeItems_)
    {
        addItem(item.second);
    }

    for (const auto& item : nodeItems_)
    {
        addItem(item.second);
    }

    update();
}

} // end namespace dcis::gui
