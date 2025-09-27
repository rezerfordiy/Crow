#pragma once

#include <QGraphicsItem>
#include <QColor>
#include <QVector>
#include <QPointF>

#include "dubins.h"

class QPainter;

class DubinsPathItem : public QGraphicsItem {
public:
    DubinsPathItem(QGraphicsItem* parent = nullptr);
    DubinsPathItem(const DubinsPath& path, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
    void setPath(const DubinsPath& path);
    bool findShortestPath(const double q0[3], const double q1[3], double rho);
    
    void setPathColor(const QColor& color);
    void setPathWidth(double width);
    
    double pathLength() const;
    DubinsPathType pathType() const;
    bool isValid() const;
    QVector<QPointF> getSampledPoints(int pointsCount = 100) const;

private:
    void calculateBoundingRect();
    QPointF samplePoint(double t) const;
    
private:
    DubinsPath _path;
    QRectF _boundingRect;
    bool _isValid;
    
    QColor _pathColor;
    double _pathWidth;
};

