#pragma once

class KglPaint;

class KvRenderable
{
public:
	KvRenderable() : visible_(true) {}

	virtual void setVisible(bool b) { visible_ = b; }

	virtual bool visible() const { return visible_; }

	virtual void draw(KglPaint*) const = 0;

private:
	bool visible_;
};
