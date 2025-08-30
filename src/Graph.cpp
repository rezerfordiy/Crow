#include "Graph.h"

#include <unordered_map>
#include <cmath>
#include "MyScene.h"

Graph::Graph(double x1, double y1, double x2, double y2) {
    Graph::Node start, end;
    start.id = 0;
    start.x = x1;
    start.y = y1;
    start.isPrimary = false;
    
    end.id = 1;
    end.x = x2;
    end.y = y2;
    end.isPrimary = false;
    nodes.push_back(start);
    nodes.push_back(end);
    
    Graph::Edge e;
    e.from = 0;
    e.to = 1;
    edges.push_back(e);
    buildAdjacencyMatrix();
    buildAdjacencyList();
}

void Graph::clear() {
    nodes.clear();
    edges.clear();
    matrix.clear();
    list.clear();
}

double Graph::abs(const Graph::Node& v1, const Graph::Node& v2) const {
    double dx = v1.x - v2.x;
    double dy = v1.y - v2.y;
    return std::sqrt(dx*dx + dy*dy);
}

void Graph::buildFromVoronoi(const VoronoiDiagram& vd,  std::vector<Obstacle> const& obstacles, std::vector<SafeZone> const& zones,
                             double startX, double startY, double endX, double endY) {
    clear();
    
    std::unordered_map<const VoronoiDiagram::vertex_type*, int> vertexIdMap;
    int vertexId = 0;
    
    for (const auto& vertex : vd.vertices()) {
        if (MyScene::isValidCoordinate(vertex.x(), vertex.y())) {
            Graph::Node v;
            v.id = vertexId;
            v.x = vertex.x();
            v.y = vertex.y();
            v.isPrimary = true;
            
            bool insideObstacle = false;
            for (const auto& obstacle : obstacles) {
                if (obstacle.isPointInside(v.x, v.y)) {
                    insideObstacle = true;
                    break;
                }
            }
            if (insideObstacle) continue;
            
            nodes.push_back(v);
            vertexIdMap[&vertex] = vertexId;
            vertexId++;
        }
    }
    
    for (const auto& edge : vd.edges()) {
        if (edge.is_finite()) {
            const auto* v0 = edge.vertex0();
            const auto* v1 = edge.vertex1();
            
            if (v0 && v1 && vertexIdMap.count(v0) && vertexIdMap.count(v1)) {
                int fromId = vertexIdMap[v0];
                int toId = vertexIdMap[v1];
                
                cordType x1 = nodes[fromId].x;
                cordType y1 = nodes[fromId].y;
                cordType x2 = nodes[toId].x;
                cordType y2 = nodes[toId].y;
                
                bool intersectsObstacle = false;
                for (const auto& obstacle : obstacles) {
                    if (obstacle.doesSegmentIntersect(x1, y1, x2, y2)) {
                        intersectsObstacle = true;
                        break;
                    }
                }
                
                if (!intersectsObstacle) {
                    double distance = abs(nodes[fromId], nodes[toId]);
                                        
                    Graph::Edge e;
                    e.from = fromId;
                    e.to = toId;
                    e.weight = distance * calculateSafetyFactor(x1, y1, x2, y2, zones);
                    
                    edges.push_back(e);
                }
            }
        }
    }
    
    const int voronCount = nodes.size();
    
    Graph::Node start, end;
    start.id = vertexId++;
    start.x = startX;
    start.y = startY;
    start.isPrimary = false;
    
    end.id = vertexId++;
    end.x = endX;
    end.y = endY;
    end.isPrimary = false;
    
    nodes.push_back(start);
    nodes.push_back(end);
    
    int closestToStartId = -1;
    int closestToEndId = -1;
    double minStartDist = std::numeric_limits<double>::max();
    double minEndDist = std::numeric_limits<double>::max();
    
    for (int i = 0; i < voronCount; i++) {
        double distToStart = abs(nodes[i], start);
        double distToEnd = abs(nodes[i], end);
        
        if (distToStart < minStartDist) {
            minStartDist = distToStart;
            closestToStartId = i;
        }
        
        if (distToEnd < minEndDist) {
            minEndDist = distToEnd;
            closestToEndId = i;
        }
    }
    
    if (closestToStartId != -1) {
        Graph::Edge eStart;
        eStart.from = start.id;
        eStart.to = closestToStartId;
        eStart.weight = minStartDist;
        edges.push_back(eStart);
    }
    
    if (closestToEndId != -1 ) {
        Graph::Edge eEnd;
        eEnd.from = end.id;
        eEnd.to = closestToEndId;
        eEnd.weight = minEndDist;
        edges.push_back(eEnd);
    }
    
    buildAdjacencyMatrix();
    buildAdjacencyList();
}

double Graph::calculateSafetyFactor(cordType x1, cordType y1, cordType x2, cordType y2,
                                   std::vector<SafeZone> const& zones) {
    double factor = 1.0;
    
    cordType midX = (x1 + x2) / 2;
    cordType midY = (y1 + y2) / 2;
    
    for (const auto& zone : zones) {
        if (zone.isPointInside(midX, midY)) {
            factor = std::min(factor, zone.weightMultiplier);
        } else {
            cordType dist = std::sqrt(std::pow(midX - (zone.x1+zone.x2)/2, 2) +
                                    std::pow(midY - (zone.y1+zone.y2)/2, 2));
            cordType maxDist = 100.0;
            
            if (dist < maxDist) {
                double koef = 1.0 - (dist / maxDist);
                double influence = zone.weightMultiplier +
                                      (1.0 - zone.weightMultiplier) * koef;
                factor = std::min(factor, influence);
            }
        }
    }
    
    return factor;
}


void Graph::buildAdjacencyMatrix() {
    int n = nodes.size();
    matrix.resize(n, std::vector<int>(n, 0));
    
    for (const auto& edge : edges) {
        matrix[edge.from][edge.to] = 1;
        matrix[edge.to][edge.from] = 1;
    }
}

void Graph::buildAdjacencyList() {
    int n = nodes.size();
    list.resize(n);
    
    for (const auto& edge : edges) {
        list[edge.from].emplace_back(edge.to, edge.weight);
        list[edge.to].emplace_back(edge.from, edge.weight);
    }
}

