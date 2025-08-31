#include "NetworkService.h"
#include "ProtocolHandler.h"
#include <QSettings>
#include <QThread>

NetworkService::NetworkService(SceneManager* manager, const QString& address, const QString& protocol)
    : manager_(manager), address_(address), protocol_(protocol) {
    
    workerThread_ = new QThread();
    workerThread_->setObjectName("NetworkServiceThread");
    
    protocolHandler_ = ProtocolHandler::create(protocol_, manager_, address_);
    if (protocolHandler_) {
        protocolHandler_->moveToThread(workerThread_);
    }
    
    connect(this, &NetworkService::startRequested, protocolHandler_.get(), &ProtocolHandler::start);
    connect(this, &NetworkService::stopRequested, protocolHandler_.get(), &ProtocolHandler::stop);
    
    connect(protocolHandler_.get(), &ProtocolHandler::started, this, &NetworkService::onProtocolStarted);
    connect(protocolHandler_.get(), &ProtocolHandler::stopped, this, &NetworkService::onProtocolStopped);
    connect(protocolHandler_.get(), &ProtocolHandler::errorOccurred, this, &NetworkService::onProtocolError);
    
    this->moveToThread(workerThread_);
    workerThread_->start();
}



NetworkService::~NetworkService() {
    stop();
    
    if (workerThread_->isRunning()) {
        workerThread_->quit();
        workerThread_->wait(1000);
    }
    delete workerThread_;
}



void NetworkService::start() {
    if (!running_) {
        emit startRequested();
    }
}
void NetworkService::stop() {
    if (running_) {
        emit stopRequested();
    }
}

void NetworkService::onProtocolStarted() {
    running_ = true;
    emit serviceStarted();  
    qDebug() << "Network service started with protocol:" << protocol_;
}

void NetworkService::onProtocolStopped() {
    running_ = false;
    emit serviceStopped(); 
    qDebug() << "Network service stopped";
}

void NetworkService::onProtocolError(const QString& error) {
    emit errorOccurred(error);
    qWarning() << "Network service error:" << error;
}

bool NetworkService::isRunning() const {
    return running_;
}


QString NetworkService::currentProtocol() const {
    return protocol_;
}

QStringList NetworkService::availableProtocols() const {
    return ProtocolHandler::availableProtocols();
}

std::unique_ptr<NetworkService> NetworkService::create(SceneManager* manager,
                                                     const QString& address,
                                                     const QString& protocol) {
        QSettings settings;
        qDebug() << "Settings file:" << settings.fileName();

//        
        QString selectedProtocol = protocol.isEmpty() ?
            settings.value("network/protocol", "grpc").toString() :
            protocol;
        
        qDebug() << "Selected protocol from settings:" << selectedProtocol;
                
        QString selectedAddress = address;
        if (address == "0.0.0.0:50051" && selectedProtocol == "http") {
            selectedAddress = "0.0.0.0:8080";
        } else {
            selectedProtocol = "grpc";
        }
        
        return std::make_unique<NetworkService>(manager, selectedAddress, selectedProtocol);
}
