#include "HttpProtocolHandler.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <QTcpServer>
#include <QThread>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QTcpServer>


HttpProtocolHandler::HttpProtocolHandler(SceneManager* manager, const QString& address)
    : manager_(manager) {
    qDebug() << "HttpProtocolHandler created in thread:" << QThread::currentThread();
    
    QString addr = address;
    QStringList parts = addr.split(":");
    address_ = parts[0];
    port_ = parts.size() > 1 ? parts[1].toInt() : 8080;
    server_ = new QTcpServer(this);
}

HttpProtocolHandler::~HttpProtocolHandler() {
    stop();
}

void HttpProtocolHandler::start() {
    if (running_) return;
    
    qDebug() << "Starting HTTP server in thread:" << QThread::currentThread();
    
    if (server_->listen(QHostAddress(address_), port_)) {
        running_ = true;
        qDebug() << "HTTP server listening on" << address_ << ":" << port_;
        emit started();
        
        connect(server_, &QTcpServer::newConnection, this, [this]() {
            while (server_->hasPendingConnections()) {
                QTcpSocket* socket = server_->nextPendingConnection();
                if (socket) {
                    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
                        handleRequest(socket);
                    });
                    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
                }
            }
        });
    }
}

void HttpProtocolHandler::stop() {
    running_ = false;
    if (server_) {
        server_->close();
        disconnect(server_, &QTcpServer::newConnection, this, nullptr);
    }
    emit stopped();
}

bool HttpProtocolHandler::isRunning() const {
    return running_;
}

QString HttpProtocolHandler::protocolName() const {
    return "http";
}

void HttpProtocolHandler::handleRequest(QTcpSocket* socket) {
    static QMap<QTcpSocket*, QByteArray> buffers;
    
    if (!buffers.contains(socket)) {
        buffers[socket] = QByteArray();
    }
    
    buffers[socket] += socket->readAll();
    
    if (buffers[socket].contains("\r\n\r\n")) {
        QByteArray request = buffers[socket];
        buffers.remove(socket);
        
        processHttpRequest(socket, request);
    }
}

void HttpProtocolHandler::processHttpRequest(QTcpSocket* socket, const QByteArray& request) {
    qDebug() << "HTTP request received, length:" << request.length();
    qDebug() << "Request:" << request.left(200) << "...";
    
    QList<QByteArray> lines = request.split('\n');
    if (lines.isEmpty()) {
        sendResponse(socket, 400, "Bad Request");
        return;
    }
    
    QList<QByteArray> requestLine = lines[0].trimmed().split(' ');
    if (requestLine.size() < 2) {
        sendResponse(socket, 400, "Bad Request");
        return;
    }
    
    QByteArray method = requestLine[0];
    QByteArray path = requestLine[1];
    
    qDebug() << "Method:" << method << "Path:" << path;
    
    if (method == "GET" && path.startsWith("/scene")) {
        try {
            auto config = manager_->getSceneConfig();
            
            QJsonObject json;
            QJsonObject start;
            start["x"] = config.start().x();
            start["y"] = config.start().y();
            json["start"] = start;
            
            QJsonObject end;
            end["x"] = config.end().x();
            end["y"] = config.end().y();
            json["end"] = end;
            
            QJsonArray obstaclesArray;
            for (const auto& obstacle : config.obstacles()) {
                QJsonObject ob;
                QJsonObject tl;
                tl["x"] = obstacle.topleft().x();
                tl["y"] = obstacle.topleft().y();
                ob["topleft"] = tl;
                
                QJsonObject br;
                br["x"] = obstacle.bottomright().x();
                br["y"] = obstacle.bottomright().y();
                ob["bottomright"] = br;
                
                obstaclesArray.append(ob);
            }
            json["obstacles"] = obstaclesArray;
            
            QJsonArray safeZonesArray;
            for (const auto& zone : config.safezones()) {
                QJsonObject sz;
                QJsonObject tl;
                tl["x"] = zone.topleft().x();
                tl["y"] = zone.topleft().y();
                sz["topleft"] = tl;
                
                QJsonObject br;
                br["x"] = zone.bottomright().x();
                br["y"] = zone.bottomright().y();
                sz["bottomright"] = br;
                
                safeZonesArray.append(sz);
            }
            json["safezones"] = safeZonesArray;
            
            QJsonDocument doc(json);
            sendResponse(socket, 200, doc.toJson(QJsonDocument::Indented));
        } catch (const std::exception& e) {
            qDebug() << "Exception in GET /scene:" << e.what();
            sendResponse(socket, 500, "Internal Server Error");
        } catch (...) {
            qDebug() << "Unknown exception in GET /scene";
            sendResponse(socket, 500, "Internal Server Error");
        }
    }
    else if (method == "POST" && path.startsWith("/scene")) {
        try {
            QByteArray body = extractRequestBody(request);
            qDebug() << "POST body:" << body;
            
            QJsonDocument doc = QJsonDocument::fromJson(body);
            
            if (doc.isNull()) {
                sendResponse(socket, 400, "Invalid JSON");
                return;
            }
            
            QJsonObject json = doc.object();
            scene::SceneConfig config;
            
            if (json.contains("start")) {
                QJsonObject start = json["start"].toObject();
                config.mutable_start()->set_x(start["x"].toDouble());
                config.mutable_start()->set_y(start["y"].toDouble());
            }
            
            if (json.contains("end")) {
                QJsonObject end = json["end"].toObject();
                config.mutable_end()->set_x(end["x"].toDouble());
                config.mutable_end()->set_y(end["y"].toDouble());
            }
            
            if (json.contains("obstacles")) {
                QJsonArray obstacles = json["obstacles"].toArray();
                for (const auto& ob : obstacles) {
                    QJsonObject obstacle = ob.toObject();
                    auto rect = config.add_obstacles();
                    rect->mutable_topleft()->set_x(obstacle["topleft"].toObject()["x"].toDouble());
                    rect->mutable_topleft()->set_y(obstacle["topleft"].toObject()["y"].toDouble());
                    rect->mutable_bottomright()->set_x(obstacle["bottomright"].toObject()["x"].toDouble());
                    rect->mutable_bottomright()->set_y(obstacle["bottomright"].toObject()["y"].toDouble());
                }
            }
            
            if (json.contains("safezones")) {
                QJsonArray safeZones = json["safezones"].toArray();
                for (const auto& sz : safeZones) {
                    QJsonObject zone = sz.toObject();
                    auto rect = config.add_safezones();
                    rect->mutable_topleft()->set_x(zone["topleft"].toObject()["x"].toDouble());
                    rect->mutable_topleft()->set_y(zone["topleft"].toObject()["y"].toDouble());
                    rect->mutable_bottomright()->set_x(zone["bottomright"].toObject()["x"].toDouble());
                    rect->mutable_bottomright()->set_y(zone["bottomright"].toObject()["y"].toDouble());
                }
            }
            
            manager_->updateFromConfig(config);
            sendResponse(socket, 200, "OK");
        } catch (const std::exception& e) {
            qDebug() << "Exception in POST /scene:" << e.what();
            sendResponse(socket, 400, "Bad Request");
        } catch (...) {
            qDebug() << "Unknown exception in POST /scene";
            sendResponse(socket, 400, "Bad Request");
        }
    }
    else if (method == "POST" && path.startsWith("/clear")) {
        try {
            manager_->clearScene();
            sendResponse(socket, 200, "OK");
        } catch (...) {
            sendResponse(socket, 500, "Internal Server Error");
        }
    } else {
        sendResponse(socket, 404, "Not Found");
    }
    
    socket->close();
}

void HttpProtocolHandler::sendResponse(QTcpSocket* socket, int status, const QByteArray& content) {
    QString statusText;
    switch (status) {
        case 200: statusText = "OK"; break;
        case 400: statusText = "Bad Request"; break;
        case 404: statusText = "Not Found"; break;
        case 500: statusText = "Internal Server Error"; break;
        default: statusText = "Unknown"; break;
    }
    
    QByteArray response = "HTTP/1.1 " + QByteArray::number(status) + " " + statusText.toUtf8() + "\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + QByteArray::number(content.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "\r\n";
    response += content;
    
    qDebug() << "Sending response, length:" << response.length();
    socket->write(response);
    socket->flush();
}

QByteArray HttpProtocolHandler::extractRequestBody(const QByteArray& httpRequest) {
    int headerEnd = httpRequest.indexOf("\r\n\r\n");
    if (headerEnd == -1) {
        return QByteArray();
    }
    return httpRequest.mid(headerEnd + 4);
}
