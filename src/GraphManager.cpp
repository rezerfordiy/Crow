#include <QMessageBox>
#include <QCoreApplication>

#include "GraphManager.h"
#include "SceneData.h"

GraphManager::GraphManager(QObject* parent) : QObject(parent) {}


void GraphManager::build(SceneData const* data) {
    std::vector<Point> points;
    for (auto const& ob : data->getObstacles()) {
        auto pnts = ob.getPoints();
        points.insert(points.end(), pnts.begin(), pnts.end());
    }
    
    Obstacle fake(data->getTopleft().x() - 50, data->getTopleft().y() - 50, data->getBottomright().x() + 50, data->getBottomright().y() + 50);
    auto borderPoints = fake.getPoints();
    points.insert(points.end(), borderPoints.begin(), borderPoints.end());
    
    if (points.empty()) { return; }
    
    try {
        VoronoiDiagram vd;
        boost::polygon::construct_voronoi(points.begin(), points.end(), &vd);
        graph.buildFromVoronoi(vd, data);
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", "Ошибка при построении диаграммы");
        QCoreApplication::exit(1);
    }
}
