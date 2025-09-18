#pragma once

class Graph;

class AStar {
public:
    std::vector<int> operator()(int from, int to, const Graph& graph) const;
};
