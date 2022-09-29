#pragma once
#include "eccentricity.h"
#include <assert.h>


/*
 * The radius of a graph is the minimum eccentricity of
 * any vertex in that graph.
 */

template<typename GRAPH>
unsigned radius(const GRAPH& g)
{
	assert(g.order() > 0);
	unsigned r = -1;

	for (unsigned v = 0; v < g.order(); v++) {
		auto ecc = eccentricity(g, v);
		if (ecc < r)
			r = ecc;
	}

	return r;
}
