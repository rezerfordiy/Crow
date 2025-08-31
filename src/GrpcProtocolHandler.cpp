#include "GrpcProtocolHandler.h"
#include <QDebug>
#include <QThread>
#include <grpcpp/server_builder.h>
#include <thread>


GrpcProtocolHandler::ServiceImpl::ServiceImpl(SceneManager* manager) : manager_(manager) {}
    
grpc::Status GrpcProtocolHandler::ServiceImpl::UpdateScene(grpc::ServerContext* context,
                        const scene::SceneConfig* request,
                        scene::SceneResponse* response)  {
    qDebug() << "gRPC UpdateScene called in thread:" << QThread::currentThread();
    
    try {
        manager_->updateFromConfig(*request);
        response->set_success(true);
        response->set_message("Scene updated successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return grpc::Status::OK;
}

grpc::Status GrpcProtocolHandler::ServiceImpl::GetScene(grpc::ServerContext* context,
                    const scene::Empty* request,
                    scene::SceneConfig* response)  {
    qDebug() << "gRPC GetScene called in thread:" << QThread::currentThread();
    
    try {
        *response = manager_->getSceneConfig();
    } catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
    return grpc::Status::OK;
}

grpc::Status GrpcProtocolHandler::ServiceImpl::ClearScene(grpc::ServerContext* context,
                    const scene::Empty* request,
                    scene::SceneResponse* response)  {
    qDebug() << "gRPC ClearScene called in thread:" << QThread::currentThread();
    
    try {
        manager_->clearScene();
        response->set_success(true);
        response->set_message("Scene cleared successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return grpc::Status::OK;
}
    

GrpcProtocolHandler::GrpcProtocolHandler(SceneManager* manager, const QString& address)
    : manager_(manager), address_(address), serviceImpl_(std::make_unique<ServiceImpl>(manager)) {
    qDebug() << "GrpcProtocolHandler created in thread:" << QThread::currentThread();
}

GrpcProtocolHandler::~GrpcProtocolHandler() {
    stop();
}

void GrpcProtocolHandler::start() {
    if (running_) return;
    
    //  to not access from another thread
    std::lock_guard<std::mutex> lock(serverMutex_);
    qDebug() << "Starting gRPC server in thread:" << QThread::currentThread();
    
    grpc_init();
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address_.toStdString(), grpc::InsecureServerCredentials());
    builder.RegisterService(serviceImpl_.get());
    
    server_ = builder.BuildAndStart();
    if (server_) {
        running_ = true;
        qDebug() << "gRPC server listening on " << address_;
        emit started();
        
        server_->Wait();
    }
}

void GrpcProtocolHandler::stop() {
    std::lock_guard<std::mutex> lock(serverMutex_);
    if (server_) {
        server_->Shutdown();
        server_.reset();
    }
    running_ = false;
    grpc_shutdown();
    emit stopped();
}

bool GrpcProtocolHandler::isRunning() const {
    return running_;
}

QString GrpcProtocolHandler::protocolName() const {
    return "grpc";
}
