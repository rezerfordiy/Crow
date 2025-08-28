#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

class MyScene;

#include "scene.grpc.pb.h"

class SceneServiceImpl final : public scene::SceneService::Service {
public:
    SceneServiceImpl(MyScene* scene) : scene_(scene) {}
    
    grpc::Status UpdateScene(grpc::ServerContext* context,
                            const scene::SceneConfig* request,
                            scene::SceneResponse* response) override;
    
    grpc::Status GetScene(grpc::ServerContext* context,
                         const scene::Empty* request,
                         scene::SceneConfig* response) override;
    
    grpc::Status ClearScene(grpc::ServerContext* context,
                           const scene::Empty* request,
                           scene::SceneResponse* response) override;

private:
    MyScene* scene_;
};

class SceneServer {
public:
    SceneServer(MyScene* scene, const std::string& address = "0.0.0.0:50051");
    ~SceneServer();
    
    void Start();
    void Stop();
    
private:
    MyScene* scene_;
    std::unique_ptr<grpc::Server> server_;
    std::string address_;
};
