#include <unordered_map>
#include <cmath>

#include "SceneData.h"
#include "Graph.h"

Graph::Graph() {
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

void Graph::buildFromVoronoi(const VoronoiDiagram& vd, SceneData const* data) {
    clear();
    
    auto obstacles = data->getObstacles();
    auto zones = data->getSafeZones();
    auto startPoint = data->getStartPoint();
    auto endPoint = data->getEndPoint();
    
    auto startX = startPoint.x();
    auto startY = startPoint.y();
    auto endX = endPoint.x();
    auto endY = endPoint.y();
    
    std::unordered_map<const VoronoiDiagram::vertex_type*, int> vertexIdMap;
    int vertexId = 0;
    
    processVoronoiVertices(vd, obstacles, vertexIdMap, vertexId);
    processVoronoiEdges(vd, obstacles, zones, vertexIdMap);
    
    const int voronCount = nodes.size();
    
    addStartEndNodes(startX, startY, endX, endY, vertexId);
    connectStartEndToGraph(voronCount, startX, startY, endX, endY);
    
    buildAdjacencyMatrix();
    buildAdjacencyList();
}

void Graph::processVoronoiVertices(const VoronoiDiagram& vd,
                                  const std::vector<Obstacle>& obstacles,
                                  std::unordered_map<const VoronoiDiagram::vertex_type*, int>& vertexIdMap,
                                  int& vertexId) {
    for (const auto& vertex : vd.vertices()) {
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

void Graph::processVoronoiEdges(const VoronoiDiagram& vd,
                               const std::vector<Obstacle>& obstacles,
                               const std::vector<SafeZone>& zones,
                               std::unordered_map<const VoronoiDiagram::vertex_type*, int>& vertexIdMap) {
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
}

void Graph::addStartEndNodes(cordType startX, cordType startY, cordType endX, cordType endY, int& vertexId) {
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
}

void Graph::connectStartEndToGraph(int voronCount, cordType startX, cordType startY, cordType endX, cordType endY) {
    int closestToStartId = -1;
    int closestToEndId = -1;
    double minStartDist = std::numeric_limits<double>::max();
    double minEndDist = std::numeric_limits<double>::max();
    
    Graph::Node startProxy;
    startProxy.x = startX;
    startProxy.y = startY;
    
    Graph::Node endProxy;
    endProxy.x = endX;
    endProxy.y = endY;
    
    for (int i = 0; i < voronCount; i++) {
        double distToStart = abs(nodes[i], startProxy);
        double distToEnd = abs(nodes[i], endProxy);
        
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
        eStart.from = nodes[nodes.size() - 2].id;
        eStart.to = closestToStartId;
        eStart.weight = minStartDist;
        edges.push_back(eStart);
    }
    
    if (closestToEndId != -1) {
        Graph::Edge eEnd;
        eEnd.from = nodes[nodes.size() - 1].id;
        eEnd.to = closestToEndId;
        eEnd.weight = minEndDist;
        edges.push_back(eEnd);
    }
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
    const int n = nodes.size();
    matrix.resize(n, std::vector<int>(n, 0));
    
    for (const auto& edge : edges) {
        matrix[edge.from][edge.to] = 1;
        matrix[edge.to][edge.from] = 1;
    }
}

void Graph::buildAdjacencyList() {
    const int n = nodes.size();
    list.resize(n);
    
    for (const auto& edge : edges) {
        list[edge.from].emplace_back(edge.to, edge.weight);
        list[edge.to].emplace_back(edge.from, edge.weight);
    }
}

