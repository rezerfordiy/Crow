#include "AStar.h"
#include <queue>



std::vector<int> AStar::findPath(int from, int to, Graph const& g) {
    if (from < 0 || from >= g.nodes.size() || to < 0 || to >= g.nodes.size()) {
        return {};
    }
    
    auto cmp = [](std::pair<int, double> left, std::pair<int, double> right) {
        return left.second > right.second;
    };
    
    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(cmp)> q(cmp);
    
    std::vector<double> gScore(g.nodes.size(), std::numeric_limits<double>::max());
    std::vector<int> cameFrom(g.nodes.size(), -1);
    
    gScore[from] = 0;
    q.push({from, g.abs(g.nodes[from], g.nodes[to])}); // f = g + h
    
    while (!q.empty()) {
        int current = q.top().first;
        double fCurrent = q.top().second;
        q.pop();
        
        if (current == to) {
            break;
        }
        
        if (fCurrent > gScore[current] + g.abs(g.nodes[current], g.nodes[to])) {
            continue;
        }
        
        for (const auto& neighborInfo : g.list[current]) {
            int neighbor = neighborInfo.first;
            double edgeCost = neighborInfo.second;
            
            double newGScore = gScore[current] + edgeCost;
            
            if (newGScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = newGScore;
                
                double fScore = newGScore + g.abs(g.nodes[neighbor], g.nodes[to]);
                q.push({neighbor, fScore});
            }
        }
    }
    
    if (cameFrom[to] == -1) {
        return {};
    }
    
    std::vector<int> path;
    int current = to;
    while (current != from) {
        path.push_back(current);
        current = cameFrom[current];
    }
    path.push_back(from);
    std::reverse(path.begin(), path.end());
    
    return path;
}
