#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "Graph.h"
#include "Obstacle.h"
#include "SafeZone.h"

#include <memory>
#include "SceneService.h"

class QGraphicsLineItem;
class QGraphicsEllipseItem;

class MyScene : public QGraphicsScene
{
    Q_OBJECT

public:
    static int width;
    static int height;
    static std::vector<Segment> borders;
    
    int mode = 0;
    
    
    explicit MyScene(QObject *parent = nullptr);
    
    void clearAll();
    void drawBorder();
    
    std::vector<Obstacle> getObstacles() const;
    std::vector<SafeZone> getSafeZones() const;
    QPointF getStartPoint() const;
    QPointF getEndPoint() const;
    
    void addObstacle(double posx, double posy, double width = 40, double height = 40);
    void addObstacle(Obstacle const&);
    
    void addSafeZone(double posx, double posy, double width = 40, double height = 40, double mult = 0.3);
    void addSafeZone(SafeZone const&);
    
    void addDistanation(double posx, double posy);
    
public slots:
    void updateSceneFromConfig(const scene::SceneConfig& config);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

public:
    std::vector<Point> points;
    std::vector<QGraphicsLineItem*> way;
    std::vector<QGraphicsEllipseItem*> voronoiNodes;
    std::vector<QGraphicsLineItem*> voronoiEdges;
    std::vector<QGraphicsLineItem*> borderItems;
    std::vector<Obstacle> obstacles;
    std::vector<SafeZone> zones;
    
    
    QGraphicsEllipseItem* start;
    QGraphicsEllipseItem* end;
    
    Graph g;
    
    
    void rebuildVoronoiDiagram();
    void rebuildVoronoiDiagram(std::vector<Segment> const&);
    void makeGraph(VoronoiDiagram const&);
    void clearVoronoiEdgesNodes();
public:
    
    std::unique_ptr<SceneServer> sceneServer;
    bool isStart = true;
    static bool isValidCoordinate(double x, double y);
};
