#pragma once


#include <boost/polygon/voronoi.hpp>
#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>


typedef double cordType;
typedef boost::polygon::point_data<cordType> Point;
typedef boost::polygon::segment_data<cordType> Segment;
typedef boost::polygon::voronoi_diagram<cordType> VoronoiDiagram;

