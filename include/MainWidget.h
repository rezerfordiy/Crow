#pragma once

#include <QWidget>

class MyScene;
class ScrollableView;


class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
public slots:
    void centerOn();
private:
    // ONLY FIRST U MUST REMEMBER
    MyScene* scene;
    ScrollableView* view;
};

