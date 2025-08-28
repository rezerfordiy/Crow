#pragma once

#include <QWidget>

class MyScene;
class QGraphicsView;
class QPushButton;
class QSpinBox;

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);

private slots:
    void clearScene();
private:
    // ONLY FIRST U MUST REMEMBER
    MyScene* scene;
    QGraphicsView* view;
    QPushButton* clearButton;
    QSpinBox* changeSceneMode;
};
