#pragma once

#include <QWidget>

class MyScene;
class ScrollableView;
class GraphManager;
class NetworkManager;
class SceneData;
class QJsonObject;


class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
public slots:
    void centerOn();
    void updateFromJson(const QJsonObject& json);
private:
    // ONLY FIRST U MUST REMEMBER
    MyScene* scene;
    ScrollableView* view;
    GraphManager* graphManager;
    NetworkManager* networkManager;
    SceneData* data;
};

