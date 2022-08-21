#include "KvThemedPlot.h"
#include <memory>
#include <QBrush>


class QCustomPlot;
class QCPAbstractPlottable;
class QCPLegend;

class KcThemedQCP : public KvThemedPlot
{
public:
	KcThemedQCP(std::shared_ptr<QCustomPlot> qcp); // 接管qcp控制权
	virtual ~KcThemedQCP();

	QBrush fill(int level) const override;
	void applyFill(int level, const QBrush&) override;

	QPen border(int level) const override;
	void applyBorder(int level, const QPen&) override;

	QMargins margins(int level) const override;
	void applyMargins(int level, const QMargins&) override;

	bool visible(int level) const override;
	void applyVisible(int level, bool b) override;

	void applyLine(int level, std::function<QPen(const QPen&)> op) override;
	void applyText(int level, std::function<QFont(const QFont&)> op) override;
	void applyTextColor(int level, std::function<QColor(const QColor&)> op) override;

	void setTickLength(int level, KeTickSide side, int len) override;

	unsigned numPlots() const override;
	void applyPalette(unsigned plotIdx, const QColor& major, const QColor& minor) override;

	KeLegendPlacement legendPlacement() override;
	void setLegendPlacement(KeLegendPlacement lp) override;

	int legendAlignment() override;
	void setLegendAlignment(int la) override;

	KeLegendArrangement legendArrangement() override;
	void setLegendArrangement(KeLegendArrangement la) override;

	std::pair<int, int> legendSpacing() override;
	void setLegendSpacing(int xspacing, int yspacing) override;

	// some helper methods

	bool legendVisible() const;
	void setLegendVisible(bool b);

	static void applyPalette(QCPAbstractPlottable* plot, const QColor& major, const QColor& minor);

private:

	void applyAxisVisible_(int level, bool b);
	//void applyGridVisible_(int level, bool b); TODO:

	QCPLegend* takeLegend_();
	void putLegend_(QCPLegend* legend, KeLegendPlacement place, int align);

private:
	std::shared_ptr<QCustomPlot> qcp_;
	QBrush bkgnd_;
	int legendAlign;
};

