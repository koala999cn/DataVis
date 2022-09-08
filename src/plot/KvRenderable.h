#pragma once

class KglPaint;

class KvRenderable
{
public:

	virtual void setVisible(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void draw(KglPaint*) const = 0;

};
