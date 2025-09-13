#pragma once

#include <QGraphicsScene>

#include "SceneData.h"
#include "Graph.h"
#include "NetworkManager.h"


class MyScene : public QGraphicsScene {
    Q_OBJECT
public:
    MyScene(QObject* parent = nullptr);
    
public slots:
    void updateFromJson(const QJsonObject& json);
    
signals:
    void centerRequested();

private:
    std::unique_ptr<SceneData> data;
    std::unique_ptr<Graph> graph;
    std::unique_ptr<NetworkManager> networkManager;
        
    static std::pair<double, double> geographicToUtm(double longitude, double latitude);
    
    void rebuildVoronoiDiagram();
    void drawData();
};
