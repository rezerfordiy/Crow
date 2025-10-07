#pragma once
#include <vector>

#include <QGraphicsScene>
#include <QVector>

class QGraphicsItem;
class Graph;
class SceneData;
class Obstacle;
class QGraphicsSceneMousePressEvent;
class MyEllipseItem;

class MyScene : public QGraphicsScene {
    Q_OBJECT
public:
    MyScene(QObject* parent = nullptr);
    void drawGraph(Graph const*);
    QVector<QGraphicsItem*> drawPath(std::vector<int> const& path, Graph const*, const std::vector<Obstacle>& obstacles);
    void drawData(SceneData const* data);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
signals:
    void centerRequested();
    void pointChosen(MyEllipseItem* item);
};
