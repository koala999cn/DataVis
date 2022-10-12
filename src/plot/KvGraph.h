#pragma once
#include "KvPlottable.h"
#include "KpContext.h"
#include <assert.h>


// 折线图的基类

class KvGraph : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override {
		return 1;
	}

	bool minorColorNeeded() const override {
		return false;
	}

	const color4f& majorColor(unsigned idx) const override {
		assert(idx == 0);
		return cxt_.color;
	}

	color4f& majorColor(unsigned idx) override {
		assert(idx == 0);
		return cxt_.color;
	}

	const color4f& minorColor() const override {
		assert(false); // we'll never reach here
		return cxt_.color; // just make compiler happy
	}

	color4f& minorColor() override {
		assert(false); // we'll never reach here
		return cxt_.color; // just make compiler happy
	}

private:
	KpLineContext cxt_;
};
