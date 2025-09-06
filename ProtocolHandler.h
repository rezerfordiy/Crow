#pragma once

#include "SceneManager.h"
#include <QObject>

class ProtocolHandler : public QObject {
    Q_OBJECT
    
public:
    virtual ~ProtocolHandler() = default;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual QString protocolName() const = 0;
    
    static std::unique_ptr<ProtocolHandler> create(const QString& protocol, SceneManager* manager, const QString& address);
    static QStringList availableProtocols();

signals:
    void started();
    void stopped();
    void errorOccurred(const QString& error); 
};
