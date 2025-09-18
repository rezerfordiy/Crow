
#include "Obstacle.h"

cordType Obstacle::dt = 20;


Obstacle::Obstacle(cordType _x1, cordType _y1, cordType _x2, cordType _y2) : x1(std::min(_x1, _x2)), y1(std::min(_y1, _y2)),
x2(std::max(_x1, _x2)), y2(std::max(_y1, _y2)) {}

std::vector<Segment> Obstacle::getSegments() const {
   std::vector<Segment> segments;
   segments.push_back(Segment(Point(x1, y1), Point(x2, y1)));
   segments.push_back(Segment(Point(x2, y1), Point(x2, y2)));
   segments.push_back(Segment(Point(x2, y2), Point(x1, y2)));
   segments.push_back(Segment(Point(x1, y2), Point(x1, y1)));
   return segments;
}


std::vector<Point> Obstacle::getPoints() const {
    std::vector<Point> ans;
        
    ans.emplace_back(x1, y1);
    ans.emplace_back(x2, y1);
    ans.emplace_back(x2, y2);
    ans.emplace_back(x1, y2);
    
    if (dt > 0) {
        for (cordType x = x1 + dt; x < x2; x += dt) {
            ans.emplace_back(x, y1);
        }
        for (cordType y = y1 + dt; y < y2; y += dt) {
            ans.emplace_back(x2, y);
        }
        for (cordType x = x2 - dt; x > x1; x -= dt) {
            ans.emplace_back(x, y2);
        }
        for (cordType y = y2 - dt; y > y1; y -= dt) {
            ans.emplace_back(x1, y);
        }
    }
    
    std::sort(ans.begin(), ans.end());
    ans.erase(std::unique(ans.begin(), ans.end()), ans.end());
    
    return ans;
}


bool Obstacle::doesSegmentIntersect(cordType x1, cordType y1, cordType x2, cordType y2) const {
    if (isPointInside(x1, y1) && isPointInside(x2, y2)) {
        return true;
    }
    
    return checkLineSegmentIntersection(x1, y1, x2, y2, this->x1, this->y1, this->x2, this->y1) ||
           checkLineSegmentIntersection(x1, y1, x2, y2, this->x2, this->y1, this->x2, this->y2) ||
           checkLineSegmentIntersection(x1, y1, x2, y2, this->x2, this->y2, this->x1, this->y2) ||
           checkLineSegmentIntersection(x1, y1, x2, y2, this->x1, this->y2, this->x1, this->y1);
}



bool Obstacle::isPointInside(cordType x, cordType y) const {
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}


// stolen
bool Obstacle::checkLineSegmentIntersection(cordType p0_x, cordType p0_y, cordType p1_x, cordType p1_y,
                                 cordType p2_x, cordType p2_y, cordType p3_x, cordType p3_y) const {
    cordType s1_x = p1_x - p0_x;
    cordType s1_y = p1_y - p0_y;
    cordType s2_x = p3_x - p2_x;
    cordType s2_y = p3_y - p2_y;

    cordType s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    cordType t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}
