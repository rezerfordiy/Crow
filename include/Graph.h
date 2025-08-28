#pragma once


#include "Types.h"
#include "Obstacle.h"
#include "SafeZone.h"
#include <vector>


class Graph {
public:
    struct Node {
        int id;
        double x, y;
        bool isPrimary;
        
    };

    struct Edge {
        int from, to;
        double weight;
        double safetyWeight = 1.0;
    };
    
    Graph(double,double,double,double);
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> matrix;
    std::vector<std::vector<std::pair<int, double>>> list;
    
    
    void buildFromVoronoi(const VoronoiDiagram& vd,  std::vector<Obstacle> const& obstacles,  std::vector<SafeZone> const& zones,
                          double x1, double y1, double x2,  double y2);
    
    double calculateSafetyFactor(cordType x1, cordType y1, cordType x2, cordType y2, std::vector<SafeZone> const& safe_zones);
    void clear();
    
    
    
    
    
    double abs(const Node& v1, const Node& v2) const;
    void buildAdjacencyMatrix();
    void buildAdjacencyList();
};
