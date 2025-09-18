
#pragma once

#include <vector>

#include <QPointF>
#include <QObject>

#include "Obstacle.h"
#include "SafeZone.h"

class SceneData: public QObject {
    Q_OBJECT
public:
    static std::pair<double, double> geographicToUtm(double longitude, double latitude);

    bool isFirstUpdate = true;
    bool isStart = true;
    
    std::vector<Obstacle> obstacles;
    std::vector<SafeZone> zones;
    QPointF startPoint;
    QPointF endPoint;
    QPointF topleft;
    QPointF bottomright;
    
    SceneData(QObject* parent = nullptr);
    
    void clear();
    void updateBorders(const QPointF& point);
    void updateBorders(double x, double y);
    void updateBorders(std::pair<double, double>);
    
    void addObstacle(double x1, double y1, double x2, double y2, bool isUtm);
    void addObstacle(double posx, double posy, double width = 40, double height = 40);
    
    void addSafeZone(double x1, double y1, double x2, double y2, double mult, bool isUtm);
    void addSafeZone(double posx, double posy, double width = 40, double height = 40, double mult = 0.3);
    
    void addDistanation(double posx, double posy, bool isUtm);
    void addDistanation(double posx, double posy);
    
    QPointF getTopleft() const;
    QPointF getBottomright() const;
    
    std::vector<Obstacle> getObstacles() const;
    std::vector<SafeZone> getSafeZones() const;
    QPointF getStartPoint() const;
    QPointF getEndPoint() const;
};
