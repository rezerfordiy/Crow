
#include <boost/geometry.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/srs/epsg.hpp>
#ifdef COLIRU
#include <boost/geometry/srs/projection.hpp>
#endif

#include "SceneData.h"

namespace bg = boost::geometry;
namespace srs = bg::srs;

SceneData::SceneData(QObject* parent) : QObject(parent), startPoint(0, 0), endPoint(800, 400) {}

void SceneData::clear() {
    obstacles.clear();
    zones.clear();
    topleft = QPointF(0, 0);
    bottomright = QPointF(800, 400);
}

void SceneData::updateBorders(const QPointF& p) {
    if (isFirstUpdate) {
        topleft = p;
        bottomright = p;
        isFirstUpdate = false;
        return;
    }
    topleft.setX(qMin(topleft.x(), p.x()));
    topleft.setY(qMin(topleft.y(), p.y()));
    bottomright.setX(qMax(bottomright.x(), p.x()));
    bottomright.setY(qMax(bottomright.y(), p.y()));
}

void SceneData::updateBorders(double x, double y) {
    updateBorders(QPointF(x, y));
}

void SceneData::updateBorders(std::pair<double, double> para) {
    updateBorders(para.first, para.second);
}


// from stackoverflow
std::pair<double, double> SceneData::geographicToUtm(double longitude, double latitude) {
    if (longitude < -180.0 || longitude > 180.0) {
        return {-1, -1};
    }
    if (latitude < -90.0 || latitude > 90.0) {
        return {-1, -1};
    }
    
    typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> geographicPoint;
    typedef bg::model::point<double, 2, bg::cs::cartesian> cartesianPoint;
    
    geographicPoint gP(longitude, latitude);
    
    int zone = static_cast<int>((longitude + 180.0) / 6.0) + 1;
    
    bool north = (latitude >= 0.0);
    
    std::string projStr = "+proj=utm +zone=" + std::to_string(zone) +
                             (north ? " +north" : " +south") +
                             " +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
    
    srs::projection<> proj = srs::proj4(projStr);
    cartesianPoint uP;
    
    proj.forward(gP, uP);
    return {bg::get<0>(uP), bg::get<1>(uP)};
}

void SceneData::addObstacle(double x1, double y1, double x2, double y2, bool isUtm) {
    if (isUtm) {
        addObstacle(x1, y1, x2 - x1, y2 - y1);
    } else {
        auto p1 = geographicToUtm(x1, y1);
        auto p2 = geographicToUtm(x2, y2);
        addObstacle(p1.first, p1.second, p2.first - p1.first, p2.second - p1.second);
        updateBorders(p1);
        updateBorders(p2);
    }
    
}

void SceneData::addObstacle(double posx, double posy, double width, double height) {
    Obstacle ob(posx, posy, posx + width, posy + height);
    obstacles.push_back(ob);
}

void SceneData::addSafeZone(double x1, double y1, double x2, double y2, double mult, bool isUtm) {
    if (isUtm) {
        addSafeZone(x1, y1, x2 - x1, y2 - y1, mult);
    } else {
        auto p1 = geographicToUtm(x1, y1);
        auto p2 = geographicToUtm(x2, y2);
        addSafeZone(p1.first, p1.second, p2.first - p1.first, p2.second - p1.second, mult);
        updateBorders(p1);
        updateBorders(p2);
    }
    
}


void SceneData::addSafeZone(double posx, double posy, double width, double height, double mult) {
    SafeZone z(posx, posy, posx + width, posy + height, mult);
    zones.push_back(z);
}

void SceneData::addDistanation(double posx, double posy, bool isUtm) {
    if (isUtm) {
        addDistanation(posx, posy);
    } else {
        auto p = geographicToUtm(posx, posy);
        addDistanation(p.first, p.second);
        updateBorders(p);
    }
}

void SceneData::addDistanation(double posx, double posy) {
    if (isStart) {
        startPoint = QPointF(posx, posy);
    } else {
        endPoint = QPointF(posx, posy);
    }
    isStart = !isStart;
}

QPointF SceneData::getTopleft() const {
    return topleft;
}
QPointF SceneData::getBottomright() const {
    return bottomright;
}

std::vector<Obstacle> SceneData::getObstacles() const {
    return obstacles;
}
std::vector<SafeZone> SceneData::getSafeZones() const {
    return zones;
}
QPointF SceneData::getStartPoint() const {
    return startPoint;
}
QPointF SceneData::getEndPoint() const {
    return endPoint;
}
