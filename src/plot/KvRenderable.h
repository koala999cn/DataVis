#pragma once
#include "KtAABB.h"

class KvPaint;

class KvRenderable
{
public:
	KvRenderable() : visible_(true) {}

	virtual void setVisible(bool b) { visible_ = b; }

	virtual bool visible() const { return visible_; }

	virtual void draw(KvPaint&) const = 0;

	virtual KtAABB<double> boundingBox() const = 0;

private:
	bool visible_;
};
