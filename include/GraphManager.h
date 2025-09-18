
#pragma once

#include <QObject>

#include "Graph.h"

class GraphManager : public QObject {
    Q_OBJECT
public:
    GraphManager(QObject* parent = nullptr);
    void build(SceneData const* data); 

    Graph graph;
};
