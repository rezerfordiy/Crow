#include <QPainter>

#include "DubinsPathItem.h"

DubinsPathItem::DubinsPathItem(QGraphicsItem* parent) : DubinsPathItem(DubinsPath(), false, parent) {
    _path.qi[0] = _path.qi[1] = _path.qi[2] = 0;
    _path.param[0] = _path.param[1] = _path.param[2] = 0;
    _path.rho = 1.0;
    _path.type = LSL;
}

DubinsPathItem::DubinsPathItem(const DubinsPath& path, QGraphicsItem* parent) : DubinsPathItem(path, true, parent) {}

DubinsPathItem::DubinsPathItem(const DubinsPath& path, bool isValid, QGraphicsItem* parent) : QGraphicsItem(parent), _path(path), _isValid(isValid),_pathColor(Qt::green), _pathWidth(3.0) {
    
    calculateBoundingRect();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
}


void DubinsPathItem::calculateBoundingRect() {
    if (!_isValid) {
        _boundingRect = QRectF(-10, -10, 20, 20);
        return;
    }
    
    QVector<QPointF> points = getSampledPoints(50);
    if (points.isEmpty()) {
        _boundingRect = QRectF(-10, -10, 20, 20);
        return;
    }
    
    double minX = points[0].x();
    double maxX = points[0].x();
    double minY = points[0].y();
    double maxY = points[0].y();
    
    for (const QPointF& point : points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    
    double padding = 25.0;
    _boundingRect = QRectF(minX - padding, minY - padding,
                           maxX - minX + 2 * padding, maxY - minY + 2 * padding);
}

QRectF DubinsPathItem::boundingRect() const {
    return _boundingRect;
}

void DubinsPathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (!_isValid) {
        painter->setPen(Qt::red);
        painter->drawText(boundingRect(), Qt::AlignCenter, "Invalid Dubins Path");
        return;
    }
    
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    if (isSelected()) {
        painter->setPen(QPen(QColor(255, 200, 0), 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
    
    QVector<QPointF> pathPoints = getSampledPoints(100);
    if (pathPoints.size() > 1) {
        QPen pathPen(_pathColor);
        pathPen.setWidthF(_pathWidth);
        pathPen.setCapStyle(Qt::RoundCap);
        pathPen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pathPen);
        painter->drawPolyline(pathPoints.data(), pathPoints.size());
    }
    
}

QPointF DubinsPathItem::samplePoint(double t) const {
    double q[3];
    DubinsPath pathCopy = _path;
    if (dubins_path_sample(&pathCopy, t, q) == EDUBOK) {
        return QPointF(q[0], q[1]);
    }
    return QPointF();
}

QVector<QPointF> DubinsPathItem::getSampledPoints(int pointsCount) const {
    QVector<QPointF> points;
    if (!_isValid) return points;
    
    DubinsPath pathCopy = _path;
    double length = dubins_path_length(&pathCopy);
    if (length <= 0) return points;
    
    for (int i = 0; i <= pointsCount; ++i) {
        double t = (length * i) / pointsCount;
        double q[3];
        
        if (dubins_path_sample(&pathCopy, t, q) == EDUBOK) {
            points.append(QPointF(q[0], q[1]));
        }
    }
    
    return points;
}

void DubinsPathItem::setPath(const DubinsPath& path) {
    prepareGeometryChange();
    _path = path;
    _isValid = true;
    calculateBoundingRect();
    update();
}

bool DubinsPathItem::findShortestPath(const double q0[3], const double q1[3], double rho) {
    prepareGeometryChange();
    int result = dubins_shortest_path(&_path, const_cast<double*>(q0), const_cast<double*>(q1), rho);
    _isValid = (result == EDUBOK);
    calculateBoundingRect();
    update();
    return _isValid;
}

double DubinsPathItem::pathLength() const {
    if (!_isValid) return 0.0;
    
    DubinsPath pathCopy = _path;
    return dubins_path_length(&pathCopy);
}

DubinsPathType DubinsPathItem::pathType() const {
    return _path.type;
}

bool DubinsPathItem::isValid() const {
    return _isValid;
}

void DubinsPathItem::setPathColor(const QColor& color) {
    _pathColor = color;
    update();
}


void DubinsPathItem::setPathWidth(double width) {
    _pathWidth = width;
    update();
}
