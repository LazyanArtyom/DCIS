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
#ifndef DCIS_COMMON_GRAPH_NODE_H_
#define DCIS_COMMON_GRAPH_NODE_H_

// App includes

// Qt includes
#include <QPointF>

// STL includes
#include <list>
#include <string>
#include <unordered_set>

namespace dcis::common::graph
{

class Node;
using NodeListType = std::list<Node *>;
using NodePairType = std::pair<Node *, Node *>;
using NodeSetType  = std::unordered_set<Node>;
using EdgeSetType  = std::unordered_set<NodePairType>;

class Node
{
  public:
    explicit Node(const std::string &name);
    explicit Node(const std::string &name, QPointF euclidePos);

    enum class Type
    {
        Inner,
        Border,
        Corner,
        None
    };

    enum class Category
    {
        Generic,
        Drone,
        Target,
        Attacker,
    };

    QPointF getEuclidePos() const;
    void setEuclidePos(QPointF euclidePos);

    void setType(Type nodeType);
    Type getType() const;

    void setCategory(Category nodeCategory);
    Category getCategory() const;

    void incPosDegree();
    void incNegDegree();
    void decPosDegree();
    void decNegDegree();
    void incUndirDegree();
    void decUndirDegree();

    int getNegDegree() const;
    int getPosDegree() const;
    int getDirDegree() const;
    int getUndirDegree() const;

    double getX() const;
    double getY() const;
    void setX(double x);
    void setY(double y);
    std::string getName() const;

    bool operator==(const Node &other) const
    {
        return (name_ == other.name_);
    }
    bool operator<(const Node &other) const
    {
        return (name_ < other.name_);
    }
    bool operator>(const Node &other) const
    {
        return (name_ > other.name_);
    }

  private:
    int deg_ = 0;
    int degPos_ = 0;
    int degNeg_ = 0;
    bool drone_ = false;

    std::string name_;
    QPointF euclidePos_;

    Type nodeType_ = Type::Inner;
    Category nodeCategory_ = Category::Generic;
};

} // end namespace dcis::common::graph

template <> struct std::hash<dcis::common::graph::Node>
{
    size_t operator()(const dcis::common::graph::Node &node) const
    {
        return hash<std::string>()(node.getName());
    }
};

#endif // DCIS_COMMON_GRAPH_NODE_H_
