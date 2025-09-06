#include "MyScene.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPen>
#include <iostream>
#include <QGraphicsTextItem>
#include <QString>
#include "AStar.h"
#include <QGraphicsRectItem>
#include <boost/geometry.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <boost/geometry/srs/epsg.hpp>
#ifdef COLIRU
#include <boost/geometry/srs/projection.hpp>
#endif
#include <cmath>
#include <thread>

#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

int MyScene::width = 800;
int MyScene::height = 400;
std::vector<Segment> MyScene::borders = {
    Segment(Point(0, 0), Point(width, 0)),
    Segment(Point(width, 0), Point(width, height)),
    Segment(Point(width, height), Point(0, height)),
    Segment(Point(0, height), Point(0, 0))
};

MyScene::MyScene(QObject *parent) : QGraphicsScene(parent), g(0, 0, width, height), topleft(0, 0), bottomright(width, height),
    manager(new  QNetworkAccessManager(this)) {
    
    setSceneRect(topleft.x() - 50, topleft.y() - 50, bottomright.x() - topleft.x() + 50, bottomright.y() - topleft.y() + 50);
    drawBorder();
    
    start = new QGraphicsEllipseItem(0 - 3, 0 - 3, 6, 6);
    end = new QGraphicsEllipseItem(width - 3, height - 3, 6, 6);
    
    start->setBrush(Qt::black);
    start->setPen(Qt::NoPen);
    addItem(start);
    
    end->setBrush(Qt::black);
    end->setPen(Qt::NoPen);
    addItem(end);
    
    connect(this, &MyScene::jsonReceived, this, &MyScene::updateFromJson);
    
    sendHttpRequest();
    
    
    
//
//    sceneManager = SceneManager::create(this);
//        
//        connect(sceneManager.get(), &SceneManager::requestClearScene,
//                this, &MyScene::handleClearScene, Qt::QueuedConnection);
//        
//        connect(sceneManager.get(), &SceneManager::requestUpdateScene,
//                this, &MyScene::handleUpdateScene, Qt::QueuedConnection);
//        
//        connect(sceneManager.get(), &SceneManager::requestAddObstacle,
//                this, &MyScene::handleAddObstacle, Qt::QueuedConnection);
//        
//        connect(sceneManager.get(), &SceneManager::requestAddSafeZone,
//                this, &MyScene::handleAddSafeZone, Qt::QueuedConnection);
//        
//        connect(sceneManager.get(), &SceneManager::requestSetStartPoint,
//                this, &MyScene::handleSetStartPoint, Qt::QueuedConnection);
//        
//        connect(sceneManager.get(), &SceneManager::requestSetEndPoint,
//                this, &MyScene::handleSetEndPoint, Qt::QueuedConnection);
//        
//    
//    networkService = NetworkService::create(sceneManager.get(), "0.0.0.0:50051", "http");
//    
//    connect(networkService.get(), &NetworkService::serviceStarted,
//            this, &MyScene::onNetworkServiceStarted);
//    connect(networkService.get(), &NetworkService::serviceStopped,
//            this, &MyScene::onNetworkServiceStopped);
//    connect(networkService.get(), &NetworkService::errorOccurred,
//            this, &MyScene::onNetworkServiceError);
//    
//    networkService->start();
}


void MyScene::drawBorder() {
    return;
    for (QGraphicsLineItem* border : borderItems) {
        removeItem(border);
        delete border;
    }
    borderItems.clear();
    
    for (const auto& segment : borders) {
        int x1 = segment.low().x();
        int y1 = segment.low().y();
        int x2 = segment.high().x();
        int y2 = segment.high().y();
              
        QGraphicsLineItem* borderLine = new QGraphicsLineItem(x1, y1, x2, y2);
              
        QPen pen(Qt::red);
        pen.setWidth(3);
        borderLine->setPen(pen);
        addItem(borderLine);
        borderItems.push_back(borderLine);
    }
}

std::vector<Obstacle> MyScene::getObstacles() const { return obstacles; }
std::vector<SafeZone> MyScene::getSafeZones() const { return zones; }
QPointF MyScene::getStartPoint() const { return start->rect().center(); }
QPointF MyScene::getEndPoint() const { return end->rect().center(); }

void MyScene::handleClearScene() {
    clearAll();
}

//void MyScene::handleUpdateScene(const scene::SceneConfig& config) {
//    updateSceneFromConfig(config);
//}

void MyScene::handleAddObstacle(double x1, double y1, double width, double height) {
    addObstacle(x1, y1, width, height);
}

void MyScene::handleAddSafeZone(double x1, double y1, double width, double height, double mult) {
    addSafeZone(x1, y1, width, height, mult);
}

void MyScene::handleSetStartPoint(double x, double y) {
    bool wasStart = isStart;
    isStart = true;
    addDistanation(x, y);
    isStart = wasStart;
}

void MyScene::handleSetEndPoint(double x, double y) {
    bool wasStart = isStart;
    isStart = false;
    addDistanation(x, y);
    isStart = wasStart;
}

void MyScene::addObstacle(double x1, double y1, double x2, double y2, bool isUtm) {
    if (isUtm) {
        addObstacle(x1, y1, x2 - x1, y2 - y1);
    } else {
        auto p1 = geographicToUtm(x1, y1);
        auto p2 = geographicToUtm(x2, y2);
        addObstacle(p1.first, p1.second, p2.first - p1.first, p2.second - p1.second);
    }
    
}

void MyScene::addObstacle(double posx, double posy, double width, double height) {
    
    Obstacle ob(posx, posy, posx + width, posy + height);
    ob.graphics = new QGraphicsRectItem(posx, posy, width, height);
    ob.graphics->setBrush(Qt::red);
    addItem(ob.graphics);
    addObstacle(ob);
}


void MyScene::addObstacle(Obstacle const& ob) { obstacles.push_back(ob); }


void MyScene::addSafeZone(double x1, double y1, double x2, double y2, double mult, bool isUtm) {
    if (isUtm) {
        addSafeZone(x1, y1, x2 - x1, y2 - y1, mult);
    } else {
        auto p1 = geographicToUtm(x1, y1);
        auto p2 = geographicToUtm(x2, y2);
        addSafeZone(p1.first, p1.second, p2.first - p1.first, p2.second - p1.second, mult);
    }
    
}


void MyScene::addSafeZone(double posx, double posy, double width, double height, double mult) {
    SafeZone z(posx, posy, posx + width, posy + height, mult);
    z.graphics = new QGraphicsRectItem(posx, posy, width, height);
    z.graphics->setBrush(Qt::yellow);
    addItem(z.graphics);
    addSafeZone(z);
}
void MyScene::addSafeZone(SafeZone const& z) { zones.push_back(z); }

void MyScene::addDistanation(double posx, double posy, bool isUtm) {
    if (isUtm) {
        addDistanation(posx, posy);
    } else {
        auto p =geographicToUtm(posx, posy);
        addDistanation(p.first, p.second);
    }
}



void MyScene::addDistanation(double posx, double posy) {
    if (isStart) {
        removeItem(start);
        delete start;
        start = new QGraphicsEllipseItem(posx - 3, posy - 3, 6, 6);
        start->setBrush(Qt::black);
        start->setPen(Qt::NoPen);
        addItem(start);
    } else {
        removeItem(end);
        delete end;
        end = new QGraphicsEllipseItem(posx - 3, posy - 3, 6, 6);
        end->setBrush(Qt::black);
        end->setPen(Qt::NoPen);
        addItem(end);
    }
    isStart = !isStart;
}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }
    
    if (event->button() == Qt::RightButton) {
        double x = event->scenePos().x();
        double y = event->scenePos().y();
            
        if (mode == 1) {
            addObstacle(x, y);
        } else if (mode == 0) {
            addDistanation(x, y);
        } else if (mode == 2) {
            addSafeZone(x, y);
        }
        rebuildVoronoiDiagram();
        event->accept();
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MyScene::rebuildVoronoiDiagram() {
    std::vector<Segment> all; // = borders;
    
    for (auto const& ob : obstacles) {
        auto segments = ob.getSegments();
        all.insert(all.end(), segments.begin(), segments.end());
    }
    
    rebuildVoronoiDiagram(all);
}

void MyScene::rebuildVoronoiDiagram(std::vector<Segment> const& segments) {
    clearVoronoiEdgesNodes();
    setSceneRect(topleft.x() - 50, topleft.y() - 50, bottomright.x() - topleft.x() + 50, bottomright.y() - topleft.y() + 50);

    if (segments.empty()) return;
    
    try {
        VoronoiDiagram vd;
        
        boost::polygon::construct_voronoi(segments.begin(), segments.end(), &vd);
        
        makeGraph(vd);
        
        int startId = g.nodes.size() - 2;
        int endId = g.nodes.size() - 1;
        
        auto path = AStar::findPath(startId, endId, g);
        
        if (!path.empty()) {
            for (int i = 1; i < path.size(); i++) {
                int fromId = path[i-1];
                int toId = path[i];
                
                double x1 = g.nodes[fromId].x;
                double y1 = g.nodes[fromId].y;
                double x2 = g.nodes[toId].x;
                double y2 = g.nodes[toId].y;
                
                QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
                QPen pen(Qt::green);
                pen.setWidth(3);
                line->setPen(pen);
                line->setZValue(10);
                addItem(line);
                way.push_back(line);
            }
        }
        
        for (const auto& node : g.nodes) {
            QGraphicsEllipseItem *point = new QGraphicsEllipseItem(node.x - 2, node.y - 2, 4, 4);
            point->setBrush(Qt::blue);
            point->setPen(Qt::NoPen);
            addItem(point);
            voronoiNodes.push_back(point);
        }
        
        for (const auto& edge : g.edges) {
            double x1 = g.nodes[edge.from].x;
            double y1 = g.nodes[edge.from].y;
            double x2 = g.nodes[edge.to].x;
            double y2 = g.nodes[edge.to].y;
            
            QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
            QPen pen(Qt::blue);
            pen.setWidth(1);
            line->setPen(pen);
            addItem(line);
            voronoiEdges.push_back(line);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error building Voronoi diagram: " << e.what() << std::endl;
    }
}

void MyScene::makeGraph(VoronoiDiagram const& vd) {
    g.clear();
    g.buildFromVoronoi(vd, obstacles, zones, start->rect().center().x(), start->rect().center().y(),
                      end->rect().center().x(), end->rect().center().y());
}

void MyScene::clearVoronoiEdgesNodes() {
    for (QGraphicsLineItem* edge : voronoiEdges) {
        removeItem(edge);
        delete edge;
    }
    for (QGraphicsLineItem* edge : way) {
        removeItem(edge);
        delete edge;
    }
    for (QGraphicsEllipseItem* n : voronoiNodes) {
        removeItem(n);
        delete n;
    }
    voronoiEdges.clear();
    voronoiNodes.clear();
    way.clear();
}

void MyScene::clearAll() {
    clearVoronoiEdgesNodes();
    
    QList<QGraphicsItem*> items = this->items();
    for (QGraphicsItem* item : items) {
        bool isBorder = false;
        for (QGraphicsLineItem* border : borderItems) {
            if (item == border) {
                isBorder = true;
                break;
            }
        }
        
        if (!isBorder) {
            if (item->type() == QGraphicsEllipseItem::Type && item != start && item != end) {
                removeItem(item);
                delete item;
            }
            else if (item->type() == QGraphicsLineItem::Type) {
                removeItem(item);
                delete item;
            }
        }
    }
    
    
    for (auto& ob : obstacles) {
        removeItem(ob.graphics);
        delete ob.graphics;
    }
    obstacles.clear();
    
    for (auto& z : zones) {
        removeItem(z.graphics);
        delete z.graphics;
    }
    
    zones.clear();
    
    drawBorder();
}
bool MyScene::isValidCoordinate(double x, double y)  {
    return true;
}


void MyScene::sendHttpRequest() {
    QUrl url("http://127.0.0.1:8080/get_route");
    QNetworkRequest request(url);
    
    QNetworkReply *reply = manager->get(request);
    
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            if (!jsonDoc.isNull()) {
                emit jsonReceived(jsonDoc.object());
            } else {
                QMessageBox::critical(nullptr, "Ошибка", "Пустой json");
                QCoreApplication::exit(1);
            }
        } else {
            qDebug() << "Error:" << reply->errorString();
            QMessageBox::critical(nullptr, "Ошибка", "Нет сети");
            QCoreApplication::exit(1);
        }
        reply->deleteLater();
    });
}

void MyScene::updateFromJson(const QJsonObject& json) {
    clearAll();
    bool UTM = false;
    _isFirst = true;
    try {
        if (json.contains("start")) {
            QJsonObject start = json["start"].toObject();
            addDistanation(start["x"].toDouble(), start["y"].toDouble(), UTM);
            updateBorders(start["x"].toDouble(), start["y"].toDouble());
        }
        
        if (json.contains("end")) {
            QJsonObject end = json["end"].toObject();
            addDistanation(end["x"].toDouble(), end["y"].toDouble(), UTM);
            updateBorders(end["x"].toDouble(), end["y"].toDouble());
        }
        
        if (json.contains("obstacles")) {
            QJsonArray obstacles = json["obstacles"].toArray();
            for (const auto& ob : obstacles) {
                QJsonObject obstacle = ob.toObject();
                addObstacle(
                            obstacle["topleft"].toObject()["x"].toDouble(),
                            obstacle["topleft"].toObject()["y"].toDouble(),
                            obstacle["bottomright"].toObject()["x"].toDouble(),
                            obstacle["bottomright"].toObject()["y"].toDouble(),
                            UTM
                            );
                updateBorders(
                              obstacle["topleft"].toObject()["x"].toDouble(),
                              obstacle["topleft"].toObject()["y"].toDouble()
                              );
                updateBorders(
                              obstacle["bottomright"].toObject()["x"].toDouble(),
                              obstacle["bottomright"].toObject()["y"].toDouble()
                              );
            }
        }
        
        if (json.contains("safezones")) {
            QJsonArray safeZones = json["safezones"].toArray();
            for (const auto& sz : safeZones) {
                QJsonObject zone = sz.toObject();
                addSafeZone(
                            zone["topleft"].toObject()["x"].toDouble(),
                            zone["topleft"].toObject()["y"].toDouble(),
                            zone["bottomright"].toObject()["x"].toDouble(),
                            zone["bottomright"].toObject()["y"].toDouble(),
                            0.3,
                            UTM
                            );
                updateBorders(
                              zone["topleft"].toObject()["x"].toDouble(),
                              zone["topleft"].toObject()["y"].toDouble()
                              );
                updateBorders(
                              zone["bottomright"].toObject()["x"].toDouble(),
                              zone["bottomright"].toObject()["y"].toDouble()
                              );

            }
        }
        
        setSceneRect(topleft.x() - 50, topleft.y() - 50, bottomright.x() - topleft.x() + 50, bottomright.y() - topleft.y() + 50);
        rebuildVoronoiDiagram();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", "Ошибка при обработке сцены");
        QCoreApplication::exit(1);
    } catch (...) {
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестные ошибки");
        QCoreApplication::exit(1);
    }
}

///SERVICE
//void MyScene::updateSceneFromConfig(const scene::SceneConfig& config) {
//    clearAll();
//    
//    bool UTM = false;
//    
//    _isFirst = true;
//    
//    if (config.has_start()) {
//        addDistanation(config.start().x(), config.start().y(), UTM);
//    }
//    
//    if (config.has_end()) {
//        addDistanation(config.end().x(), config.end().y(), UTM);
//    }
//    
//    for (const auto& obstacle : config.obstacles()) {
//        addObstacle(obstacle.topleft().x(), obstacle.topleft().y(),
//                             obstacle.bottomright().x(), obstacle.bottomright().y(), UTM);
//    }
//    
//    for (const auto& zone : config.safezones()) {
//        addSafeZone(zone.topleft().x(), zone.topleft().y(),
//                    zone.bottomright().x(), zone.bottomright().y(), 0.3, UTM);
//    }
//    _isFirst = false;
//    rebuildVoronoiDiagram();
//    emit centerRequested();
//}





namespace bg = boost::geometry;
namespace srs = bg::srs;


// from stackoverflow
std::pair<double, double> MyScene::geographicToUtm(double longitude, double latitude) {
    if (longitude < -180.0 || longitude > 180.0) {
        return {-1, -1};
    }
    if (latitude < -90.0 || latitude > 90.0) {
        return {-1, -1};
    }
    
    typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> geographicPoint;
    typedef bg::model::point<double, 2, bg::cs::cartesian> cartesianPoint;
    
    geographicPoint gP(longitude, latitude);
    
    int zone = static_cast<int>((longitude + 180.0) / 6.0) + 1;
    
    bool north = (latitude >= 0.0);
    
    std::string projStr = "+proj=utm +zone=" + std::to_string(zone) +
                             (north ? " +north" : " +south") +
                             " +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
    
    srs::projection<> proj = srs::proj4(projStr);
    cartesianPoint uP;
    
    proj.forward(gP, uP);
    
    
    

    return {bg::get<0>(uP), bg::get<1>(uP)};
}


//std::unique_ptr<SceneManager> MyScene::createSceneManager() {
//    return std::make_unique<MySceneManager>(this);
//}

void MyScene::updateBorders(double x, double y, bool first) {
    updateBorders(QPointF(x, y), first);

}

void MyScene::updateBorders(std::pair<double, double> const& p, bool first) {
    updateBorders(QPointF(p.first, p.second), first);

}
void MyScene::updateBorders(QPointF const& p, bool first) {
    if (_isFirst) {
        _isFirst = false;
        topleft = p;
        bottomright = p;
        return;
    }
    topleft.setX(qMin(topleft.x(), p.x()));
    topleft.setY(qMin(topleft.y(), p.y()));
    bottomright.setX(qMax(bottomright.x(), p.x() ));
    bottomright.setY(qMax(bottomright.y(), p.y() ));
}



QPointF MyScene::getTopleft() const {
    return topleft;
}
QPointF MyScene::getBottomright() const {
    return bottomright;
}
