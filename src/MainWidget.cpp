#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QApplication>

#include "MainWidget.h"
#include "MyScene.h"
#include "ScrollableView.h"
#include "GraphManager.h"
#include "NetworkManager.h"
#include "SceneData.h"
#include "AStar.h"

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
        
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
        
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
      
        scene->drawData(data);
        scene->drawPath(path, &graphManager->graph);
        scene->drawGraph(&graphManager->graph);

        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", "Ошибка при обработке сцены");
        QCoreApplication::exit(1);
    } catch (...) {
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестные ошибки");
        QCoreApplication::exit(1);
    }
}
