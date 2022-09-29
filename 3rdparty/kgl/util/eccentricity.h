#pragma once
#include <algorithm>
#include "distance.h"


/*
 * Vertex eccentricity is the length of the longest shortest path
 * from a vertex(vid) to any other vertex
 */

template<typename GRAPH>
unsigned eccentricity(const GRAPH& g, unsigned v)
{
	auto dist = distance(g, v);
	return *std::max_element(dist.cbegin(), dist.cend());
}

