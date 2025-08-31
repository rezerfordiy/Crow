#pragma once


#include "Types.h"
#include <vector>

class QGraphicsRectItem;


struct SafeZone {
    cordType x1, y1, x2, y2;
    double weightMultiplier;
    QGraphicsRectItem* graphics;
    
    SafeZone(cordType _x1, cordType _y1, cordType _x2, cordType _y2, double multiplier = 0.3);
    
    bool isPointInside(cordType x, cordType y) const;
    bool isSegmentInside(cordType x1, cordType y1, cordType x2, cordType y2) const;
    
};
