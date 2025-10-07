
#include "MyEllipseItem.h"
MyEllipseItem::MyEllipseItem(int num, QGraphicsItem* parent) : QGraphicsEllipseItem(parent), number{num} {}


MyEllipseItem::MyEllipseItem(int num, qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent) : QGraphicsEllipseItem(x,y,w,h,parent), number{num}{}
