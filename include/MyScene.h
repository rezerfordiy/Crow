#pragma once
#include <vector>

#include <QGraphicsScene>

class Graph;
class SceneData;
class Obstacle;

class MyScene : public QGraphicsScene {
    Q_OBJECT
public:
    MyScene(QObject* parent = nullptr);
    void drawGraph(Graph const*);
    void drawPath(std::vector<int> const& path, Graph const*, const std::vector<Obstacle>& obstacles);
    void drawData(SceneData const* data);

signals:
    void centerRequested();
};
