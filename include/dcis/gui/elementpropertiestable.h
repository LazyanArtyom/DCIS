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
#ifndef DCIS_GUI_ELEMENTPROPERTIESTABLE_H_
#define DCIS_GUI_ELEMENTPROPERTIESTABLE_H_

// APP includes
#include <graph/graph.h>

// QT includes
#include <QHeaderView>
#include <QTableWidget>

namespace dcis::gui
{
using namespace common;

class ElementPropertiesTable : public QTableWidget
{
    Q_OBJECT
  public:
    explicit ElementPropertiesTable(graph::Graph *graph);
    explicit ElementPropertiesTable(graph::Graph *graph, int sectionSize);

  public slots:
    void onUnSelected();
    void onGraphChanged();
    void onSetGraph(graph::Graph *graph);
    void onNodeSelected(const std::string &nodeName);
    void onEdgeSelected(const std::string &uName, const std::string &vName);

  private:
    void clearTable();
    graph::Graph *graph_;
};

} // end namespace dcis::gui
#endif // DCIS_GUI_ELEMENTPROPERTIESTABLE_H_
