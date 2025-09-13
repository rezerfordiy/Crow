#pragma once

#include <QGraphicsView>

class QGraphicsScene;
class QWidget;

class ScrollableView : public QGraphicsView {
    Q_OBJECT
public:
    ScrollableView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
};
