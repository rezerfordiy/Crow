#include "MainWidget.h"
#include "MyScene.h"
#include <QPushButton>
#include "ScrollableView.h"
#include <QVBoxLayout>
#include <QSpinBox>


MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
    scene(new MyScene(this)),
    view(new ScrollableView(scene)),
    clearButton(new QPushButton("clear", this)),
    changeSceneMode(new QSpinBox(this)) {
        
    
    changeSceneMode->setRange(0, 3);
    changeSceneMode->setValue(0);
    changeSceneMode->setSingleStep(1);
        
        
    connect(scene, &MyScene::centerRequested, this, &MainWidget::centerOn);
    connect(clearButton, &QPushButton::clicked, this, &MainWidget::clearScene);
    connect(changeSceneMode, QOverload<int>::of(&QSpinBox::valueChanged),
                [this](int value) {
                    this->scene->mode = value;
                });
        
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(clearButton);
    layout->addWidget(changeSceneMode);

    setLayout(layout);
    resize(1400, 1000);
}

void MainWidget::clearScene() {
    scene->clearAll();
}

void MainWidget::centerOn() {


    QPointF center = (scene->getTopleft() + scene->getBottomright()) / 2;
    view->centerOn(center + QPointF(0, 0));
    
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setInteractive(true);
}
