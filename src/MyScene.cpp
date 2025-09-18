#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

#include "MyScene.h"
#include "Graph.h"
#include "SceneData.h"


MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {}

void MyScene::drawGraph(Graph const * graph) {
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
}

void MyScene::drawPath(std::vector<int> const& path, Graph const* graph) {
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
}


void MyScene::drawData(SceneData const* data) {
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
