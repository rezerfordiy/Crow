#pragma once

#include <QWidget>



class MyScene;
class ScrollableView;
class QPushButton;
class QSpinBox;



class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
public slots:
    void centerOn();
private slots:
    void clearScene();
private:
    // ONLY FIRST U MUST REMEMBER
    MyScene* scene;
    ScrollableView* view;
    QPushButton* clearButton;
    QSpinBox* changeSceneMode;
};
