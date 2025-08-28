#include "SceneService.h"
#include "MyScene.h"
#include <iostream>
#include <QMetaObject>

using grpc::ServerContext;
using grpc::Status;
using scene::SceneConfig;
using scene::SceneResponse;
using scene::Empty;
using scene::Rectangle;

Status SceneServiceImpl::UpdateScene(ServerContext* context,
                                   const SceneConfig* request,
                                   SceneResponse* response) {
    try {
        QMetaObject::invokeMethod(scene_, [this, request]() {
            scene_->clearAll();
            
            if (request->has_start()) {
                scene_->addDistanation(request->start().x(), request->start().y());
            }
            
            if (request->has_end()) {
                scene_->addDistanation(request->end().x(), request->end().y());
            }
            
            for (const auto& obstacle : request->obstacles()) {
                double x1 = obstacle.topleft().x();
                double y1 = obstacle.topleft().y();
                double x2 = obstacle.bottomright().x();
                double y2 = obstacle.bottomright().y();
                
                scene_->addObstacle(x1, y1, x2 - x1, y2 - y1);
            }
            
            for (const auto& zone : request->safezones()) {
                double x1 = zone.topleft().x();
                double y1 = zone.topleft().y();
                double x2 = zone.bottomright().x();
                double y2 = zone.bottomright().y();
                
                scene_->addSafeZone(x1, y1, x2 - x1, y2 - y1, 0.3);
            }
            
            scene_->rebuildVoronoiDiagram();
        }, Qt::BlockingQueuedConnection);
        
        response->set_success(true);
        response->set_message("Scene updated successfully");
        
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    
    return Status::OK;
}

Status SceneServiceImpl::GetScene(ServerContext* context,
                                 const Empty* request,
                                 SceneConfig* response) {
    try {
        auto obstacles = scene_->getObstacles();
        auto safeZones = scene_->getSafeZones();
        auto startPoint = scene_->getStartPoint();
        auto endPoint = scene_->getEndPoint();
        
        
            scene::Point* start = response->mutable_start();
            start->set_x(startPoint.x());
            start->set_y(startPoint.y());
        
        
            scene::Point* end = response->mutable_end();
            end->set_x(endPoint.x());
            end->set_y(endPoint.y());
        
        
        for (const auto& ob : obstacles) {
            Rectangle* obstacle = response->add_obstacles();
            scene::Point* topleft = obstacle->mutable_topleft();
            scene::Point* bottomright = obstacle->mutable_bottomright();
            
            topleft->set_x(ob.x1);
            topleft->set_y(ob.y1);
            bottomright->set_x(ob.x2);
            bottomright->set_y(ob.y2);
        }
        
        for (const auto& zone : safeZones) {
            Rectangle* safezone = response->add_safezones();
            scene::Point* topleft = safezone->mutable_topleft();
            scene::Point* bottomright = safezone->mutable_bottomright();
            
            topleft->set_x(zone.x1);
            topleft->set_y(zone.y1);
            bottomright->set_x(zone.x2);
            bottomright->set_y(zone.y2);
        }
        
    } catch (const std::exception& e) {
        return Status(grpc::StatusCode::INTERNAL, e.what());
    }
    
    return Status::OK;
}

Status SceneServiceImpl::ClearScene(ServerContext* context,
                                   const Empty* request,
                                   SceneResponse* response) {
    try {
        QMetaObject::invokeMethod(scene_, [this]() {
            scene_->clearAll();
        }, Qt::BlockingQueuedConnection);
        
        response->set_success(true);
        response->set_message("Scene cleared successfully");
        
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    
    return Status::OK;
}

SceneServer::SceneServer(MyScene* scene, const std::string& address)
    : scene_(scene), address_(address) {}

SceneServer::~SceneServer() {
    Stop();
}

void SceneServer::Start() {
    SceneServiceImpl service(scene_);
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address_, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    server_ = builder.BuildAndStart();
    std::cout << "Scene server listening on " << address_ << std::endl;
    
    server_->Wait();
}

void SceneServer::Stop() {
    if (server_) {
        server_->Shutdown();
    }
}
