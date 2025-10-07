#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>

#include "MainWidget.h"
#include "MyScene.h"
#include "ScrollableView.h"
#include "GraphManager.h"
#include "NetworkManager.h"
#include "SceneData.h"
#include "AStar.h"
#include "MyEllipseItem.h"


MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
    scene(new MyScene(this)),
    view(new ScrollableView(scene)),
    graphManager(new GraphManager(this)),
    networkManager(new NetworkManager(this)),
    data(new SceneData(this)) {

    connect(networkManager, &NetworkManager::jsonReceived,
                this, &MainWidget::updateFromJson);
    connect(networkManager, &NetworkManager::errorOccurred,
                [](const QString& error) {
                    QMessageBox::critical(nullptr, "Ошибка", error);
                    QCoreApplication::exit(1);
                });
        
        connect(scene, &MyScene::pointChosen, this, &MainWidget::handlePointChose);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    auto bt = new QPushButton(this);
    layout->addWidget(bt);
    
    connect(bt, &QPushButton::clicked, this, &MainWidget::cutPath);
        
    setLayout(layout);
    resize(1400, 1000);
    
    networkManager->sendHttpRequest();
}

void MainWidget::updateFromJson(const QJsonObject& json) {
    bool constexpr UTM = false;
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
        
        scene->setSceneRect(data->getTopleft().x() - 50, data->getTopleft().y() - 50, data->getBottomright().x() - data->getTopleft().x() + 50, data->getBottomright().y() - data->getTopleft().y() + 50);
        
        graphManager->build(data);
        
        int startId = graphManager->graph.nodes.size() - 2;
        int endId = graphManager->graph.nodes.size() - 1;
        auto path = AStar()(startId, endId, graphManager->graph);
      
        for (auto v:path) {
            curPath.append(v);
        }
        scene->drawData(data);
        drawnPath = scene->drawPath(path, &graphManager->graph, data->obstacles);
        scene->drawGraph(&graphManager->graph);

        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", "Ошибка при обработке сцены");
        QCoreApplication::exit(1);
    } catch (...) {
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестные ошибки");
        QCoreApplication::exit(1);
    }
}


void MainWidget::handlePointChose(MyEllipseItem* item) {
    if (!curPath.contains(item->number)) {
          return;
    }
    if (!q.empty() && ((q.last() == item) || (q.first() == item))) {return;}

    if (q.size() >= 2) {
        auto gone = q.dequeue();
        gone->setRect(graphManager->graph.nodes[gone->number].x - 2,
                      graphManager->graph.nodes[gone->number].y - 2,
                      4,
                      4
                      );
        gone->update();
    }
    q.enqueue(item);
    item->setRect(graphManager->graph.nodes[item->number].x - 5,
                  graphManager->graph.nodes[item->number].y - 5,
                  10,
                  10
                  );
    item->update();
}


void MainWidget::cutPath() {
    if (q.size() < 2) {return;}
    
    if (curPath.isEmpty()) {
        return;
    }
    
    int index1 = curPath.indexOf(q.first()->number);
    int index2 = curPath.indexOf(q.last()->number);
    
    if (index1 == -1 || index2 == -1) {
        return;
    }
    
    int startIndex = std::min(index1, index2);
    int endIndex = std::max(index1, index2);
    
    QVector<int> result;
    std::vector<int> res;
    result.reserve(curPath.size() - (endIndex - startIndex - 1));
    
    for (int i = 0; i < curPath.size(); ++i) {
        if (i <= startIndex || i >= endIndex) {
            result.append(curPath[i]);
            res.push_back(curPath[i]);
        }
    }
    
    curPath = result;

    for (auto item : drawnPath) {
        if (item->scene()) {
                item->scene()->removeItem(item);
        }
        delete item;
    }
    
    drawnPath = scene->drawPath(res, &graphManager->graph, data->obstacles);
}
