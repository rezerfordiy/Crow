#pragma once

#include "ProtocolHandler.h"


class QTcpServer;
class QTcpSocket;
class QByteArray;

class HttpProtocolHandler : public ProtocolHandler {
    Q_OBJECT
public:
    HttpProtocolHandler(SceneManager* manager, const QString& address);
    ~HttpProtocolHandler();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    QString protocolName() const override;
    
private:
    void handleRequest(QTcpSocket* socket);
    void processHttpRequest(QTcpSocket* socket, const QByteArray& request);
    void sendResponse(QTcpSocket* socket, int status, const QByteArray& content);
    QByteArray extractRequestBody(const QByteArray& httpRequest);
    
    SceneManager* manager_;
    QTcpServer* server_;
    QString address_;
    int port_;
    bool running_ = false;
};
