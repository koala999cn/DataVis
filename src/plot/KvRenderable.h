#pragma once

class KvPlot;

class KvRenderable
{
public:

	virtual void setVisible(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void update(KvPlot*) const = 0;

};
