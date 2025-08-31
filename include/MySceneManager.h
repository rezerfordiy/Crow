#pragma once

#include "SceneManager.h"

class MySceneManager : public SceneManager {
    Q_OBJECT
    
public:
    MySceneManager(MyScene* scene);
    
    void clearScene() override;
    void updateFromConfig(const scene::SceneConfig& config) override;
    scene::SceneConfig getSceneConfig() const override;
    
    std::vector<Obstacle> getObstacles() const override;
    std::vector<SafeZone> getSafeZones() const override;
    QPointF getStartPoint() const override;
    QPointF getEndPoint() const override;
    
    void addObstacle(double x1, double y1, double width, double height) override;
    void addSafeZone(double x1, double y1, double width, double height, double mult) override;
    void setStartPoint(double x, double y) override;
    void setEndPoint(double x, double y) override;
    
private:
    MyScene* scene_;
};
