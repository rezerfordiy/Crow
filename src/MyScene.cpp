#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>

#include <boost/polygon/voronoi.hpp>
#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>

#include "MyScene.h"
#include "AStar.h"

MyScene::MyScene(QObject* parent) : QGraphicsScene(parent),
        data(std::make_unique<SceneData>()),
        graph(std::make_unique<Graph>()),
        networkManager(std::make_unique<NetworkManager>(this)) {

    connect(networkManager.get(), &NetworkManager::jsonReceived,
            this, &MyScene::updateFromJson);
    connect(networkManager.get(), &NetworkManager::errorOccurred,
            [](const QString& error) {
                QMessageBox::critical(nullptr, "Ошибка", error);
                QCoreApplication::exit(1);
            });
    
    networkManager->sendHttpRequest();
}

void MyScene::updateFromJson(const QJsonObject& json) {
    bool UTM = false;
    data->isFirstUpdate = true;
    
    try {
        if (json.contains("start")) {
            QJsonObject start = json["start"].toObject();
            data->addDistanation(start["x"].toDouble(), start["y"].toDouble(), UTM);
        }
        
        if (json.contains("end")) {
            QJsonObject end = json["end"].toObject();
            data->addDistanation(end["x"].toDouble(), end["y"].toDouble(), UTM);
        }
        
        if (json.contains("obstacles")) {
            QJsonArray obstacles = json["obstacles"].toArray();
            for (const auto& ob : obstacles) {
                QJsonObject obstacle = ob.toObject();
                data->addObstacle(
                            obstacle["topleft"].toObject()["x"].toDouble(),
                            obstacle["topleft"].toObject()["y"].toDouble(),
                            obstacle["bottomright"].toObject()["x"].toDouble(),
                            obstacle["bottomright"].toObject()["y"].toDouble(),
                            UTM
                            );
            }
        }
        
        if (json.contains("safezones")) {
            QJsonArray safeZones = json["safezones"].toArray();
            for (const auto& sz : safeZones) {
                QJsonObject zone = sz.toObject();
                data->addSafeZone(
                            zone["topleft"].toObject()["x"].toDouble(),
                            zone["topleft"].toObject()["y"].toDouble(),
                            zone["bottomright"].toObject()["x"].toDouble(),
                            zone["bottomright"].toObject()["y"].toDouble(),
                            0.3,
                            UTM
                            );
            }
        }
        
        setSceneRect(data->getTopleft().x() - 50, data->getTopleft().y() - 50, data->getBottomright().x() - data->getTopleft().x() + 50, data->getBottomright().y() - data->getTopleft().y() + 50);
        
        rebuildVoronoiDiagram();
        drawData();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", "Ошибка при обработке сцены");
        QCoreApplication::exit(1);
    } catch (...) {
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестные ошибки");
        QCoreApplication::exit(1);
    }
}

void MyScene::rebuildVoronoiDiagram() {
//    std::vector<Segment> segments;
    std::vector<Point> points;
    for (auto const& ob : data->getObstacles()) {
        auto pnts = ob.getPoints();
        points.insert(points.end(), pnts.begin(), pnts.end());
    }
    
    Obstacle fake(data->getTopleft().x() - 50, data->getTopleft().y() - 50, data->getBottomright().x() + 50, data->getBottomright().y() + 50);
    auto borderPoints = fake.getPoints();
    points.insert(points.end(), borderPoints.begin(), borderPoints.end());
    
    
//        for (auto const& ob : data->getObstacles()) {
//            auto segs = ob.getSegments();
//            segments.insert(segments.end(), segs.begin(), segs.end());
//        }
    
//    if (segments.empty()) {return;}
    
    if (points.empty()) { return; }

    try {
            VoronoiDiagram vd;
            
            boost::polygon::construct_voronoi(points.begin(), points.end(), /*segments.begin(), segments.end(),*/ &vd);
            
            graph->buildFromVoronoi(vd, data.get());
        
            int startId = graph->nodes.size() - 2;
            int endId = graph->nodes.size() - 1;
            
            auto path = AStar::findPath(startId, endId, *graph.get());
            
            if (!path.empty()) {
                for (int i = 1; i < path.size(); i++) {
                    int fromId = path[i-1];
                    int toId = path[i];
                    
                    double x1 = graph->nodes[fromId].x;
                    double y1 = graph->nodes[fromId].y;
                    double x2 = graph->nodes[toId].x;
                    double y2 = graph->nodes[toId].y;
                    
                    QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
                    QPen pen(Qt::green);
                    pen.setWidth(3);
                    line->setPen(pen);
                    line->setZValue(10);
                    addItem(line);
                }
            }
            
            for (const auto& node : graph->nodes) {
                QGraphicsEllipseItem *point = new QGraphicsEllipseItem(node.x - 2, node.y - 2, 4, 4);
                point->setBrush(Qt::blue);
                point->setPen(Qt::NoPen);
                addItem(point);
            }
            
            for (const auto& edge : graph->edges) {
                double x1 = graph->nodes[edge.from].x;
                double y1 = graph->nodes[edge.from].y;
                double x2 = graph->nodes[edge.to].x;
                double y2 = graph->nodes[edge.to].y;
                
                QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
                QPen pen(Qt::blue);
                pen.setWidth(1);
                line->setPen(pen);
                addItem(line);
            }
            
        } catch (const std::exception& e) {
            QMessageBox::critical(nullptr, "Ошибка", "Ошибка при построении диаграммы");
            QCoreApplication::exit(1);        }
}

void MyScene::drawData() {
    for (auto ob : data->getObstacles()) {
        ob.graphics = new QGraphicsRectItem(ob.x1, ob.y1, ob.x2 - ob.x1, ob.y2 - ob.y1);
        ob.graphics->setBrush(Qt::red);
        addItem(ob.graphics);
    }
    
    for (auto zone : data->getSafeZones()) {
        zone.graphics = new QGraphicsRectItem(zone.x1, zone.y1, zone.x2 - zone.x1, zone.y2 - zone.y1);
        zone.graphics->setBrush(Qt::yellow);
        addItem(zone.graphics);
    }
    
    
    auto start = data->getStartPoint();
    auto st = new QGraphicsEllipseItem(start.x() - 3, start.y() - 3, 6, 6);
    st->setBrush(Qt::black);
    st->setPen(Qt::NoPen);
    addItem(st);
    
    auto end = data->getEndPoint();
    auto e = new QGraphicsEllipseItem(end.x() - 3, end.y() - 3, 6, 6);
    e->setBrush(Qt::black);
    e->setPen(Qt::NoPen);
    addItem(e);
    
}
