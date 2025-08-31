#pragma once


// all needed
#include "scene.grpc.pb.h"
#include "Obstacle.h"
#include "SafeZone.h"
#include <QPointF>
#include <vector>
#include <memory>
#include <QObject>

class SceneManager : public QObject {
    Q_OBJECT
    
public:
    virtual ~SceneManager() = default;
    
    virtual void clearScene() = 0;
    virtual void updateFromConfig(const scene::SceneConfig& config) = 0;
    virtual scene::SceneConfig getSceneConfig() const = 0;
    
    virtual std::vector<Obstacle> getObstacles() const = 0;
    virtual std::vector<SafeZone> getSafeZones() const = 0;
    virtual QPointF getStartPoint() const = 0;
    virtual QPointF getEndPoint() const = 0;
    
    virtual void addObstacle(double x1, double y1, double width, double height) = 0;
    virtual void addSafeZone(double x1, double y1, double width, double height, double mult = 0.3) = 0;
    virtual void setStartPoint(double x, double y) = 0;
    virtual void setEndPoint(double x, double y) = 0;
    
    static std::unique_ptr<SceneManager> create(class MyScene* scene);

signals:
    void requestClearScene();
    void requestUpdateScene(const scene::SceneConfig& config);
    void requestAddObstacle(double x1, double y1, double width, double height);
    void requestAddSafeZone(double x1, double y1, double width, double height, double mult);
    void requestSetStartPoint(double x, double y);
    void requestSetEndPoint(double x, double y);
};
