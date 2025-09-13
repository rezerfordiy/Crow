#pragma once

#include <vector>

#include "Types.h"

class QGraphicsRectItem;

struct Obstacle {
    static cordType dt;
    
    cordType x1, y1, x2, y2;
    QGraphicsRectItem* graphics;
    Obstacle(cordType _x1, cordType _y1, cordType _x2, cordType _y2);
    
    
    std::vector<Segment> getSegments() const;
    std::vector<Point> getPoints() const;
    
    
    bool doesSegmentIntersect(cordType x1, cordType y1, cordType x2, cordType y2) const;
        
    bool isPointInside(cordType x, cordType y) const;
        
private:
        bool checkLineSegmentIntersection(cordType, cordType, cordType, cordType,
                                          cordType, cordType, cordType, cordType) const;
    
};

