#pragma once

#include "Graph.h"

class AStar {
public:
    std::vector<int> operator()(int from, int to, const Graph& graph) const;
};
