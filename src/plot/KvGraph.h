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
		return !shareColor() && data() ? data()->channels() : 1;
	}

	bool minorColorNeeded() const override {
		return false;
	}

	const color4f& majorColor(unsigned idx) const override {
		return majorColors_[idx];
	}

	color4f& majorColor(unsigned idx) override {
		return majorColors_[idx];
	}

	const color4f& minorColor() const override {
		assert(false); // we'll never reach here
		return lineCxt_.color; // just make compiler happy
	}

	color4f& minorColor() override {
		assert(false); // we'll never reach here
		return lineCxt_.color; // just make compiler happy
	}

protected:
	KpLineContext lineCxt_;
	std::vector<color4f> majorColors_;
};
