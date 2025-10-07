#pragma once

#include <QWidget>
#include <QVector>
#include <QQueue>

class MyScene;
class ScrollableView;
class GraphManager;
class NetworkManager;
class SceneData;
class QJsonObject;
class QGraphicsItem;
class MyEllipseItem;

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
public slots:
    void updateFromJson(const QJsonObject& json);
    void handlePointChose(MyEllipseItem* item);
    void cutPath();
private:
    // ONLY FIRST U MUST REMEMBER
    MyScene* scene;
    ScrollableView* view;
    GraphManager* graphManager;
    NetworkManager* networkManager;
    SceneData* data;
    QVector<int> curPath;
    QVector<QGraphicsItem*> drawnPath;
    QQueue<MyEllipseItem*> q;
};

