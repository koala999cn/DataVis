#pragma once
#include "plot/KvGraph.h"


class KcImGraph2d : public KvGraph
{
public:

	using KvGraph::KvGraph;

	void draw(KvPaint*) const final;
};
