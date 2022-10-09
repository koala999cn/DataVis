#pragma once
#include "KtAABB.h"

class KvPaint;

class KvRenderable
{
public:
	using float_type = float;
	using aabb_tyle = KtAABB<float_type>;
	using point3 = KtPoint<float_type, 3>;
	using vec3 = point3;

	KvRenderable() : visible_(true) {}

	virtual void setVisible(bool b) { visible_ = b; }

	virtual bool visible() const { return visible_; }

	virtual void draw(KvPaint*) const = 0;

	virtual aabb_tyle boundingBox() const = 0;

private:
	bool visible_;
};
