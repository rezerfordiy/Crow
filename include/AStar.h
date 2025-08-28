#pragma once

#include "Graph.h"



class AStar {
public:
    static std::vector<int> findPath(int from, int to, const Graph& graph);
};
