
#include "ScrollableView.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QMouseEvent>



ScrollableView::ScrollableView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent) {
    
    setDragMode(QGraphicsView::ScrollHandDrag);
    setInteractive(true);
    setRenderHint(QPainter::Antialiasing);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ScrollableView::wheelEvent(QWheelEvent *event)  {
    if (event->modifiers() & Qt::ControlModifier) {
        double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    } else {
        QGraphicsView::wheelEvent(event);
    }
}


