#pragma once

#include <vector>

#include "Types.h"
#include "Obstacle.h"
#include "SafeZone.h"

class SceneData;
class unordered_map;

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
	
	Graph();
	std::vector<Node> nodes;
	std::vector<Edge> edges;
	std::vector<std::vector<int>> matrix;
	std::vector<std::vector<std::pair<int, double>>> list;
	
	void buildFromVoronoi(const VoronoiDiagram& vd, SceneData const* data);
	
	double calculateSafetyFactor(cordType x1, cordType y1, cordType x2, cordType y2, std::vector<SafeZone> const&);
	void clear();
	
	double abs(const Node& v1, const Node& v2) const;
	void buildAdjacencyMatrix();
	void buildAdjacencyList();
private:
    void processVoronoiVertices(const VoronoiDiagram& vd,
                               const std::vector<Obstacle>& obstacles,
                               std::unordered_map<const VoronoiDiagram::vertex_type*, int>& vertexIdMap,
                               int& vertexId);
    
    void processVoronoiEdges(const VoronoiDiagram& vd,
                            const std::vector<Obstacle>& obstacles,
                            const std::vector<SafeZone>& zones,
                            std::unordered_map<const VoronoiDiagram::vertex_type*, int>& vertexIdMap);
    
    void addStartEndNodes(cordType startX, cordType startY, cordType endX, cordType endY, int& vertexId);
    
    void connectStartEndToGraph(int voronCount, cordType startX, cordType startY, cordType endX, cordType endY);
};
