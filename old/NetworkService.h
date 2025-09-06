#pragma once

#include "SceneManager.h"
#include "ProtocolHandler.h"
#include <QObject>

class NetworkService : public QObject {
    Q_OBJECT
    
public:
    NetworkService(SceneManager* manager, const QString& address, const QString& protocol);
    ~NetworkService();
    
    void start();
    void stop();
    bool isRunning() const;
    QString currentProtocol() const;
    
    QStringList availableProtocols() const;
    
    static std::unique_ptr<NetworkService> create(SceneManager* manager,
                                                 const QString& address = "0.0.0.0:50051",
                                                 const QString& protocol = "");
    
signals:
    void startRequested();
    void stopRequested();
    void serviceStarted();
    void serviceStopped();
    void protocolChanged(const QString& newProtocol);
    void errorOccurred(const QString& error);
    
public slots: 
    void onProtocolStarted();
    void onProtocolStopped();
    void onProtocolError(const QString& error);
    
private:
    
    SceneManager* manager_;
    std::unique_ptr<ProtocolHandler> protocolHandler_;
    QThread* workerThread_;
    QString address_;
    QString protocol_;
    bool running_ = false;
};
