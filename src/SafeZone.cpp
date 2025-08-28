#include "SafeZone.h"


SafeZone::SafeZone(cordType _x1, cordType _y1, cordType _x2, cordType _y2, double multiplier) : x1(std::min(_x1, _x2)), y1(std::min(_y1, _y2)),
x2(std::max(_x1, _x2)), y2(std::max(_y1, _y2)), weightMultiplier(multiplier) {} 

bool SafeZone::isPointInside(cordType x, cordType y) const {
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

bool SafeZone::isSegmentInside(cordType x1, cordType y1, cordType x2, cordType y2) const {
    return isPointInside((x1+x2)/2, (y1+y2)/2);
}

