#pragma once
#include "KvThemedPlot.h"

class KvPlot;
class KcAxis;
class KcCoordPlane;

// һ����KvPlot�ӿ�ʩ��theme��ʵ��

class KcThemedPlotImpl_ : public KvThemedPlot
{
public:

	KcThemedPlotImpl_(KvPlot& plot);

	KpBrush fill(int level) const override;
	void applyFill(int level, const KpBrush&) override;

	KpPen border(int level) const override;
	void applyBorder(int level, const KpPen&) override;

	KtMargins<float> margins(int level) const override;
	void applyMargins(int level, const KtMargins<float>&) override;

	bool visible(int level) const override;
	void applyVisible(int level, bool b) override;

	void applyLine(int level, std::function<KpPen(const KpPen&)> op) override;

	void applyText(int level, std::function<KpFont(const KpFont&)> op) override;

	void applyTextColor(int level, std::function<color4f(const color4f&)> op) override;

	void setTickLength(int level, KeTickSide side, int len) override;

	unsigned plottableCount() const override;
	unsigned majorColorsNeeded(unsigned plottableIdx) const override;
	bool minorColorNeeded(unsigned plottableIdx) const override;
	void applyMajorColors(unsigned plottableIdx, const std::vector<color4f>& majors) override;
	void applyMinorColor(unsigned plottableIdx, const color4f& minor) override;

	KeLegendPlacement legendPlacement() override;
	void setLegendPlacement(KeLegendPlacement) override;

	int legendAlignment() override;
	void setLegendAlignment(int) override;

	KeLegendArrangement legendArrangement() override;
	void setLegendArrangement(KeLegendArrangement) override;

	std::pair<int, int> legendSpacing() override;
	void setLegendSpacing(int xspacing, int yspacing) override;


private:

	// ����level����axis
	void forAxis_(int level, std::function<bool(KcAxis&)> op);

	// ����level����plane
	void forPlane_(int level, std::function<bool(KcCoordPlane&)> op);

private:
	KvPlot& plot_;
};

