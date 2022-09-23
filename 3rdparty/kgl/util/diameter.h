#pragma once
#include "eccentricity.h"
#include <assert.h>


/*
 * The diameter of a graph is the maximum eccentricity of
 * any vertex in that graph.
 */

template<typename GRAPH>
unsigned diameter(const GRAPH& g)
{
	assert(g.order() > 0);
	unsigned d = 0;

	for (unsigned v = 0; v < g.order(); v++) {
		auto ecc = eccentricity(g, v);
		if (ecc > d)
			d = ecc;
	}

	return d;
}
