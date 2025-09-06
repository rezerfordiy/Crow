#include "MySceneManager.h"
#include "MyScene.h"
#include "scene.grpc.pb.h"

MySceneManager::MySceneManager(MyScene* scene) : scene_(scene) {

}

void MySceneManager::clearScene() {
    emit requestClearScene();
}

void MySceneManager::updateFromConfig(const scene::SceneConfig& config) {
    emit requestUpdateScene(config);
}

void MySceneManager::addObstacle(double x1, double y1, double width, double height) {
    emit requestAddObstacle(x1, y1, width, height);
}

void MySceneManager::addSafeZone(double x1, double y1, double width, double height, double mult) {
    emit requestAddSafeZone(x1, y1, width, height, mult);
}

void MySceneManager::setStartPoint(double x, double y) {
    emit requestSetStartPoint(x, y);
}

void MySceneManager::setEndPoint(double x, double y) {
    emit requestSetEndPoint(x, y);
}

std::vector<Obstacle> MySceneManager::getObstacles() const {
    return scene_->getObstacles();
}

std::vector<SafeZone> MySceneManager::getSafeZones() const {
    return scene_->getSafeZones();
}

QPointF MySceneManager::getStartPoint() const {
    return scene_->getStartPoint();
}

QPointF MySceneManager::getEndPoint() const {
    return scene_->getEndPoint();
}

scene::SceneConfig MySceneManager::getSceneConfig() const {
    scene::SceneConfig config;
    
    auto obstacles = scene_->getObstacles();
    auto safeZones = scene_->getSafeZones();
    auto startPoint = scene_->getStartPoint();
    auto endPoint = scene_->getEndPoint();
    
    scene::Point* start = config.mutable_start();
    start->set_x(startPoint.x());
    start->set_y(startPoint.y());
    
    scene::Point* end = config.mutable_end();
    end->set_x(endPoint.x());
    end->set_y(endPoint.y());
    
    for (const auto& ob : obstacles) {
        scene::Rectangle* obstacle = config.add_obstacles();
        scene::Point* topleft = obstacle->mutable_topleft();
        scene::Point* bottomright = obstacle->mutable_bottomright();
        
        topleft->set_x(ob.x1);
        topleft->set_y(ob.y1);
        bottomright->set_x(ob.x2);
        bottomright->set_y(ob.y2);
    }
    
    for (const auto& zone : safeZones) {
        scene::Rectangle* safezone = config.add_safezones();
        scene::Point* topleft = safezone->mutable_topleft();
        scene::Point* bottomright = safezone->mutable_bottomright();
        
        topleft->set_x(zone.x1);
        topleft->set_y(zone.y1);
        bottomright->set_x(zone.x2);
        bottomright->set_y(zone.y2);
    }
    
    return config;
}
