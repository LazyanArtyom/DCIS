#ifndef DCIS_COMMON_UTILS_GRAPHUTILS_H_
#define DCIS_COMMON_UTILS_GRAPHUTILS_H_

// App includes
#include <streambuf>

// Qt includes
#include <QTextEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


namespace dcis::common::utils::graph {


QString fromJSON(QJsonDocument jsonDoc)
{
    QJsonObject json = jsonDoc.object();

    bool idDirected = json.value("directed").toBool();

    for (const auto& node : json.value("nodes").toArray())
    {
        QString name = node.toObject().value("name").toString();
        double x = node.toObject().value("x").toDouble();
        double y = node.toObject().value("y").toDouble();
    }

    for (const auto& node : json.value("edges").toArray())
    {
        QString start = node.toObject().value("start").toString();
        QString end = node.toObject().value("end").toString();
    }

    return "aaaaaaa";
}

QJsonDocument toJSON()
{
    QJsonObject graph;

    //insert single datas first
    graph.insert("directed", true);

    // fill nodes
    QJsonArray nodes;
    for (int i = 0; i < 3; ++i)
    {
        QJsonObject node;
        node.insert("name", "Node1");
        node.insert("x", 5.5);
        node.insert("y", 6.6);

        nodes.push_back(node);
    }
    graph.insert("nodes", nodes);

    // fill edges
    QJsonArray edges;
    for (int i = 0; i < 3; ++i)
    {
        QJsonObject edge;
        edge.insert("start", "Node1");
        edge.insert("end", "Node6");

        edges.push_back(edge);
    }

    graph.insert("edges", edges);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(graph);

    return jsonDoc;
}

} // end namespace dcis::common::utils::graph
#endif // DCIS_COMMON_UTILS_GRAPHUTILS_H_
