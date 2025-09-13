#include <QPushButton>
#include <QVBoxLayout>
#include <QSpinBox>

#include "MainWidget.h"
#include "MyScene.h"
#include "ScrollableView.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
    scene(new MyScene(this)),
    view(new ScrollableView(scene)) {
        
    connect(scene, &MyScene::centerRequested, this, &MainWidget::centerOn);
        
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);

    setLayout(layout);
    resize(1400, 1000);
}

void MainWidget::centerOn() {

    view->centerOn(QPointF(0, 0));
    
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setInteractive(true);
}
