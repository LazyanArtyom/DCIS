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
#ifndef DCIS_COMMON_GRAPH_EDGE_H_
#define DCIS_COMMON_GRAPH_EDGE_H_

// App includes
#include <graph/node.h>

// STL includes

template <class T1, class T2> struct std::hash<std::pair<T1, T2>>
{
    size_t operator()(const pair<T1, T2> &p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

namespace dcis::common::graph
{

class Edge
{
  public:
    explicit Edge(EdgeSetType::iterator &it) : parNode_(*it) {}
    Edge(NodePairType &parNode) : parNode_(parNode) {}

    Node *u() const
    {
        return parNode_.first;
    }
    Node *v() const
    {
        return parNode_.second;
    }

  private:
    const NodePairType &parNode_;
};

} // end namespace dcis::common::graph
#endif // DCIS_COMMON_GRAPH_EDGE_H_
