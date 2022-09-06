#pragma once


class KvRenderer;

class KvRenderable
{
public:

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void draw(KvRenderer*) const = 0;

};
