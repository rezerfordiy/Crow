#include "MainWidget.h"
#include "MyScene.h"
#include <QPushButton>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSpinBox>


MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
    scene(new MyScene(this)),
    view(new QGraphicsView(scene)),
    clearButton(new QPushButton("clear", this)),
    changeSceneMode(new QSpinBox(this)) {
        
    changeSceneMode->setRange(0, 2);
    changeSceneMode->setValue(0);
    changeSceneMode->setSingleStep(1);
        
        
        
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
    resize(1000, 600);
}

void MainWidget::clearScene() {
    scene->clearAll();
}
