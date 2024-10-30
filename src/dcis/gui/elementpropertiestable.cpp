/*
 * Project Name: DCIS - Drone Collective Intelligence System
 * Copyright (C) 2022 Artyom Lazyan, Agit Atashyan, Davit Hayrapetyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <gui/elementpropertiestable.h>

namespace dcis::gui
{

ElementPropertiesTable::ElementPropertiesTable(graph::Graph *graph) : ElementPropertiesTable(graph, 48)
{
}

ElementPropertiesTable::ElementPropertiesTable(graph::Graph *graph, int sectionSize) : graph_(graph)
{
    setSizeAdjustPolicy(QTableWidget::AdjustToContents);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(sectionSize);
    verticalHeader()->setCascadingSectionResizes(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->hide();
}

void ElementPropertiesTable::onUnSelected()
{
    clearTable();
}

void ElementPropertiesTable::onGraphChanged()
{
    clearTable();
}

void ElementPropertiesTable::onSetGraph(graph::Graph *graph)
{
    graph_ = graph;
}

void ElementPropertiesTable::onNodeSelected(const std::string &nodeName)
{
    clearTable();
    QStringList tableHeader;
    if (graph_->isDirected())
    {
        setRowCount(3);
        tableHeader << tr("Name") << tr("Positive degree") << tr("Negative degree");
    }
    else
    {
        setRowCount(2);
        tableHeader << tr("Name") << tr("Degree");
    }

    setColumnCount(1);
    setVerticalHeaderLabels(tableHeader);

    setItem(0, 0, new QTableWidgetItem());
    item(0, 0)->setTextAlignment(Qt::AlignCenter);
    item(0, 0)->setText(QString::fromStdString(nodeName));
    item(0, 0)->setFlags(Qt::ItemIsEnabled);

    if (graph_->isDirected())
    {
        setItem(1, 0, new QTableWidgetItem());
        item(1, 0)->setTextAlignment(Qt::AlignCenter);
        item(1, 0)->setText(QString::number(graph_->getNode(nodeName)->getPosDegree()));
        item(1, 0)->setFlags(Qt::ItemIsEnabled);

        setItem(2, 0, new QTableWidgetItem());
        item(2, 0)->setTextAlignment(Qt::AlignCenter);
        item(2, 0)->setText(QString::number(graph_->getNode(nodeName)->getNegDegree()));
        item(2, 0)->setFlags(Qt::ItemIsEnabled);
    }
    else
    {
        setItem(1, 0, new QTableWidgetItem());
        item(1, 0)->setTextAlignment(Qt::AlignCenter);
        item(1, 0)->setText(QString::number(graph_->getNode(nodeName)->getUndirDegree()));
        item(1, 0)->setFlags(Qt::ItemIsEnabled);
    }
}

void ElementPropertiesTable::onEdgeSelected(const std::string &uName, const std::string &vName)
{
    clearTable();
    QStringList tableHeader;
    tableHeader << tr("From node") << tr("To node") << tr("Weight");
    setRowCount(3);
    setColumnCount(1);
    setVerticalHeaderLabels(tableHeader);

    setItem(0, 0, new QTableWidgetItem());
    item(0, 0)->setTextAlignment(Qt::AlignCenter);
    item(0, 0)->setText(QString::fromStdString(uName));
    item(0, 0)->setFlags(Qt::ItemIsEnabled);

    setItem(1, 0, new QTableWidgetItem());
    item(1, 0)->setTextAlignment(Qt::AlignCenter);
    item(1, 0)->setText(QString::fromStdString(vName));
    item(1, 0)->setFlags(Qt::ItemIsEnabled);

    setItem(2, 0, new QTableWidgetItem());
    item(2, 0)->setTextAlignment(Qt::AlignCenter);
    item(2, 0)->setFlags(Qt::ItemIsEnabled);
}

void ElementPropertiesTable::clearTable()
{
    setRowCount(0);
    setColumnCount(0);
}

} // end namespace dcis::gui
