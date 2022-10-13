#pragma once
#include <string_view>
#include "KtAABB.h"

class KvPaint;

class KvRenderable
{
public:
	using float_type = double;
	using aabb_type = KtAABB<float_type>;
	using point3 = KtPoint<float_type, 3>;
	using vec3 = point3;

	KvRenderable(const std::string_view& name) 
		: name_(name), visible_(true) {}

	const std::string& name() const { return name_; }
	std::string& name() { return name_; }

	bool visible() const { return visible_; }
	bool& visible() { return visible_; }

	virtual void draw(KvPaint*) const = 0;

	virtual aabb_type boundingBox() const = 0;

private:
	std::string name_;
	bool visible_;
};
