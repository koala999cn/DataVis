#include "KvThemedPlot.h"
#include <memory>
#include <QBrush>


class QCustomPlot;

class KcThemedQCP : public KvThemedPlot
{
public:
	KcThemedQCP(std::shared_ptr<QCustomPlot> qcp); // �ӹ�qcp����Ȩ
	virtual ~KcThemedQCP();

	QBrush background() const override;
	void setBackground(const QBrush& brush) override;

	QBrush axisBackground() const override;
	void setAxisBackground(const QBrush& brush) override;

	void applyLine(int level, std::function<QPen(const QPen&)> op) override;
	void applyText(int level, std::function<QFont(const QFont&)> op) override;
	void applyTextColor(int level, std::function<QColor(const QColor&)> op) override;

	void setTickLength(int level, KeTickSide side, int len) override;

	unsigned numPlots() const override;
	void applyPalette(unsigned plotIdx, const QColor& major, const QColor& minor) override;

	void setLineVisible(int level, bool b) override;
	bool lineVisible(int level) const override;

	void setTextVisible(int level, bool b) override;
	bool textVisible(int level) const override;

	QMargins margins() const override;
	void setMargins(const QMargins&) override;

private:
	std::shared_ptr<QCustomPlot> qcp_;
	QBrush bkgnd_;
};

