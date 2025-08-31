#pragma once

#include "ProtocolHandler.h"
#include <grpcpp/grpcpp.h>
#include "scene.grpc.pb.h"
#include <mutex>
#include <memory>

class GrpcProtocolHandler : public ProtocolHandler {
    Q_OBJECT
public:
    GrpcProtocolHandler(SceneManager* manager, const QString& address);
    ~GrpcProtocolHandler();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    QString protocolName() const override;
    
private:
    class ServiceImpl final : public scene::SceneService::Service {
    public:
        ServiceImpl(SceneManager* manager);
        
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
        SceneManager* manager_;
    };
    
    SceneManager* manager_;
    QString address_;
    std::unique_ptr<ServiceImpl> serviceImpl_; 
    std::unique_ptr<grpc::Server> server_;
    bool running_ = false;
    std::mutex serverMutex_;
};
