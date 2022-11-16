#ifndef DCIS_COMMON_GRAPH_EDGE_H_
#define DCIS_COMMON_GRAPH_EDGE_H_

// App includes
#include <graph/node.h>

// STL includes
#include <unordered_map>


template<class T1, class T2>
struct std::hash<std::pair<T1, T2>>
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
    Edge(NodePairType& parNode) : parNode_(parNode) {}
    
    Node* u() const { return parNode_.first; }
    Node* v() const { return parNode_.second; }

private:
    const NodePairType& parNode_;
};

} // end namespace dcis::common::graph
#endif // DCIS_COMMON_GRAPH_EDGE_H_
