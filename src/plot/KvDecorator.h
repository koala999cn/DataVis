#pragma once
#include "KvRenderable.h"
#include "KpContext.h"


// ��������bkgnd���ͱ߿�border����renderable
class KvDecorator : public KvRenderable
{
	using super_ = KvRenderable;

public:

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	bool showBkgnd() const { return showBkgnd_; }
	bool& showBkgnd() { return showBkgnd_; }

	const KpBrush& bkgndBrush() const { return bkgnd_; }
	KpBrush& bkgndBrush() { return bkgnd_; }

	// ���Ʊ����ͱ߿�bounding-box����
	virtual void draw(KvPaint*) const;

private:
	bool showBorder_{ true }, showBkgnd_{ false };
	KpPen border_;
	KpBrush bkgnd_;
};