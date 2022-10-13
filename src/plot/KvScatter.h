#pragma once
#include "KvPlottable.h"
#include "KpContext.h"
#include <assert.h>


// 散点图的基类

class KvScatter : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override {
		return 1;
	}

	bool minorColorNeeded() const override {
		return true;
	}

	const color4f& majorColor(unsigned idx) const override {
		assert(idx == 0);
		return scatCxt_.color;
	}

	color4f& majorColor(unsigned idx) override {
		assert(idx == 0);
		return scatCxt_.color;
	}

	const color4f& minorColor() const override {
		assert(false); // we'll never reach here
		return scatCxt_.color; // just make compiler happy
	}

	color4f& minorColor() override {
		assert(false); // we'll never reach here
		return scatCxt_.color; // just make compiler happy
	}

protected:
	KpScatterContext scatCxt_;
};
