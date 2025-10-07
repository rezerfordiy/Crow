#pragma once

#include <QGraphicsEllipseItem>

class MyEllipseItem : public QGraphicsEllipseItem {
public:
    MyEllipseItem(int num = -1, QGraphicsItem* parent = nullptr);
    MyEllipseItem(int num, qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = nullptr);
    int number = -1;
};

