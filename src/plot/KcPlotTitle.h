#pragma once
#include "KvDecoratorAligned.h"


class KcPlotTitle : public KvDecoratorAligned
{
	using super_ = KvDecoratorAligned;

public:

	KcPlotTitle(const std::string_view& title);

	virtual void draw(KvPaint*) const;

	const color4f& color() const { return color_; }
	color4f& color() { return color_; }

	const KpFont& font() const { return font_; }
	KpFont& font() { return font_; }

protected:
	size_t calcSize_(void* cxt) const override;

private:
	color4f color_{ 0, 0, 0, 1 };
	KpFont font_; // titleµÄ×ÖÌå
};
