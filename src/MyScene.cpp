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


#include <thread>


int MyScene::width = 800;
int MyScene::height = 400;
std::vector<Segment> MyScene::borders = {
    Segment(Point(0, 0), Point(width, 0)),
    Segment(Point(width, 0), Point(width, height)),
    Segment(Point(width, height), Point(0, height)),
    Segment(Point(0, height), Point(0, 0))
};

MyScene::MyScene(QObject *parent) : QGraphicsScene(parent), g(0, 0, width, height) {
    setSceneRect(0, 0, width, height);
    drawBorder();
    
    start = new QGraphicsEllipseItem(0 - 3, 0 - 3, 6, 6);
    end = new QGraphicsEllipseItem(width - 3, height - 3, 6, 6);
    
    start->setBrush(Qt::black);
    start->setPen(Qt::NoPen);
    addItem(start);
    
    end->setBrush(Qt::black);
    end->setPen(Qt::NoPen);
    addItem(end);
    
    sceneServer = std::make_unique<SceneServer>(this);
        std::thread serverThread([this]() {
            sceneServer->Start();
        });
        serverThread.detach();
}

void MyScene::drawBorder() {
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

void MyScene::addObstacle(double posx, double posy, double width, double height) {
    
    Obstacle ob(posx, posy, posx + width, posy + height);
    ob.graphics = new QGraphicsRectItem(posx, posy, width, height);
    ob.graphics->setBrush(Qt::red);
    addItem(ob.graphics);
    addObstacle(ob);
}


void MyScene::addObstacle(Obstacle const& ob) { obstacles.push_back(ob); }

void MyScene::addSafeZone(double posx, double posy, double width, double height, double mult) {
    SafeZone z(posx, posy, posx + width, posy + height, mult);
    z.graphics = new QGraphicsRectItem(posx, posy, width, height);
    z.graphics->setBrush(Qt::yellow);
    addItem(z.graphics);
    addSafeZone(z);
}
void MyScene::addSafeZone(SafeZone const& z) { zones.push_back(z); }

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
    if (event->button() == Qt::LeftButton) {
        double x = event->scenePos().x();
        double y = event->scenePos().y();
    
        if (x < 0 || x > width || y < 0 || y > height) {
            return;
        }
            
        if (mode == 1) {
            addObstacle(x, y);
                        
        } else if (mode == 0) {
            addDistanation(x, y);
        } else if (mode == 2) {
            addSafeZone(x, y);
        }
        rebuildVoronoiDiagram();
    }
    
    QGraphicsScene::mousePressEvent(event);
}

void MyScene::rebuildVoronoiDiagram() {
    std::vector<Segment> all = borders;
    
    for (auto const& ob : obstacles) {
        auto segments = ob.getSegments();
        all.insert(all.end(), segments.begin(), segments.end());
    }
    
    rebuildVoronoiDiagram(all);
}

void MyScene::rebuildVoronoiDiagram(std::vector<Segment> const& segments) {
    clearVoronoiEdgesNodes();
    
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
    return x >= -100 && x <= width + 100 && y >= -100 && y <= height + 100;
}

///SERVICE
///
void MyScene::updateSceneFromConfig(const scene::SceneConfig& config) {
    clearAll();
    
    if (config.has_start()) {
        addDistanation(config.start().x(), config.start().y());
    }
    
    if (config.has_end()) {
        addDistanation(config.end().x(), config.end().y());
    }
    
    for (const auto& obstacle : config.obstacles()) {
        addObstacle(obstacle.topleft().x(), obstacle.topleft().y(),
                             obstacle.bottomright().x() - obstacle.topleft().x(), obstacle.bottomright().y() - obstacle.topleft().y());
    }
    
    for (const auto& zone : config.safezones()) {
        addSafeZone(zone.topleft().x(), zone.topleft().y(),
                    zone.bottomright().x() - zone.topleft().x(), zone.bottomright().y() - zone.topleft().y());
    }
    
    rebuildVoronoiDiagram();
}
