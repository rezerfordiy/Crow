#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "MyScene.h"
#include "Graph.h"
#include "SceneData.h"
#include "DubinsPathItem.h"
#include "MyEllipseItem.h"

MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {}

void MyScene::drawGraph(Graph const * graph) {
    for (const auto& node : graph->nodes) {
        QGraphicsEllipseItem *point = new MyEllipseItem(node.id, node.x - 2, node.y - 2, 4, 4);
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

QVector<QGraphicsItem*> MyScene::drawPath(std::vector<int> const& path, Graph const* graph, const std::vector<Obstacle>& obstacles) {
    if (path.size() < 2) return QVector<QGraphicsItem*>();
    QVector<QGraphicsItem*> ans;
    
    for (int i = 1; i < path.size(); i++) {
        int fromId = path[i-1];
        int toId = path[i];
        
        double x1 = graph->nodes[fromId].x;
        double y1 = graph->nodes[fromId].y;
        double x2 = graph->nodes[toId].x;
        double y2 = graph->nodes[toId].y;
        
        double dx = x2 - x1;
        double dy = y2 - y1;
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance < 0.1) continue;
        
        double thetaStart = atan2(dy, dx);
        double thetaEnd = thetaStart;
        
        if (i + 1 < path.size()) {
            int nextId = path[i+1];
            double x3 = graph->nodes[nextId].x;
            double y3 = graph->nodes[nextId].y;
            double dxNext = x3 - x2;
            double dyNext = y3 - y2;
            if (sqrt(dxNext*dxNext + dyNext*dyNext) > 0.1) {
                thetaEnd = atan2(dyNext, dxNext);
            }
        }
        
        double q0[3] = {x1, y1, thetaStart};
        double q1[3] = {x2, y2, thetaEnd};
        
        
        
        DubinsPathItem* pathItem = new DubinsPathItem();
        if (pathItem->findShortestPath(q0, q1, 10)) {
            QVector<QPointF> pnts = pathItem->getSampledPoints(50);
            
            QColor clr = QColor(0, 150, 0);
            for (auto const& pnt : pnts) {
                bool insideObstacle = false;
                for (const auto& obstacle : obstacles) {
                    if (obstacle.isPointInside(pnt.x(), pnt.y())) {
                        insideObstacle = true;
                        break;
                    }
                }
                if (insideObstacle) { clr = QColor(150, 0, 0); break; }
            }
            
            pathItem->setPathColor(clr);
            pathItem->setPathWidth(2.5);
            pathItem->setZValue(10 + i);
            
            addItem(pathItem);
            ans.append(pathItem);
        } else {
            delete pathItem; 
        }
    }
    
    return ans;
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
    
    
//    auto start = data->getStartPoint();
//    auto st = new MyEllipseItem(start.x() - 2, start.y() - 2, 4, 4);
//    st->setBrush(Qt::black);
//    st->setPen(Qt::NoPen);
//    addItem(st);
//    
//    auto end = data->getEndPoint();
//    auto e = new QGraphicsEllipseItem(end.x() - 2, end.y() - 2, 4, 4);
//    e->setBrush(Qt::black);
//    e->setPen(Qt::NoPen);
//    addItem(e);
    
}



void MyScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (!event->isAccepted()) {
        QList<QGraphicsItem*> itms = items(event->scenePos());
        for (auto item : itms) {
            MyEllipseItem* it = dynamic_cast<MyEllipseItem*>(item);
            if (it) {
                emit pointChosen(it);
                event->accept();
                return;
            }
        }
        
    }
    QGraphicsScene::mousePressEvent(event);
}
