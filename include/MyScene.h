#pragma once
#include <vector>

#include <QGraphicsScene>

class Graph;
class SceneData;

class MyScene : public QGraphicsScene {
    Q_OBJECT
public:
    MyScene(QObject* parent = nullptr);
    void drawGraph(Graph const*);
    void drawPath(std::vector<int> const& path, Graph const*);
    void drawData(SceneData const* data);

signals:
    void centerRequested();
};
