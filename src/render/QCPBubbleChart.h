#pragma once
#include "qcustomplot/qcustomplot.h"


class QCPBubbleData
{
public:
	QCPBubbleData();
	QCPBubbleData(int bubbleSize);
	~QCPBubbleData();
	QCPBubbleData(const QCPBubbleData& other);
	QCPBubbleData(QCPBubbleData&& other);
	QCPBubbleData& operator=(const QCPBubbleData& other);
	QCPBubbleData& operator=(QCPBubbleData&& other);

	// getters:
	int size() const { return mData.size(); }
	QCPRange keyRange() const { return mKeyRange; }
	QCPRange valueRange() const { return mValueRange; }
	QCPRange bubbleBounds() const { return mBubbleBounds; }

	const QVector3D& at(int index) const { return mData[index]; }

	// setters:
	void setSize(int size);
	void setData(int index, double key, double value, double z);

	// non-property methods:
	void recalculateRanges();
	void clear();
	bool isEmpty() const { return mData.empty(); }

protected:
	// property members:
	QCPRange mKeyRange, mValueRange, mBubbleBounds;

	// non-property members:
	QVector<QVector3D> mData;
};


class QCPBubbleChart : public QCPAbstractPlottable
{
	Q_OBJECT
	/// \cond INCLUDE_QPROPERTIES
	//Q_PROPERTY(QCPRange dataRange READ dataRange WRITE setDataRange NOTIFY dataRangeChanged)
	//Q_PROPERTY(QCPAxis::ScaleType dataScaleType READ dataScaleType WRITE setDataScaleType NOTIFY dataScaleTypeChanged)
	//Q_PROPERTY(QCPColorGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
	//Q_PROPERTY(bool interpolate READ interpolate WRITE setInterpolate)
	//Q_PROPERTY(bool tightBoundary READ tightBoundary WRITE setTightBoundary)
	//Q_PROPERTY(QCPColorScale* colorScale READ colorScale WRITE setColorScale)
	/// \endcond
public:
	explicit QCPBubbleChart(QCPAxis* keyAxis, QCPAxis* valueAxis);
	virtual ~QCPBubbleChart() Q_DECL_OVERRIDE;

	// reimplemented virtual methods:
	virtual double selectTest(const QPointF& pos, bool onlySelectable, QVariant* details = nullptr) const Q_DECL_OVERRIDE;
	virtual QCPRange getKeyRange(bool& foundRange, QCP::SignDomain inSignDomain = QCP::sdBoth) const Q_DECL_OVERRIDE;
	virtual QCPRange getValueRange(bool& foundRange, QCP::SignDomain inSignDomain = QCP::sdBoth, const QCPRange& inKeyRange = QCPRange()) const Q_DECL_OVERRIDE;

	void setData(const QCPBubbleData& data);
	void setData(QCPBubbleData&& data);

	void setBubbleSize(int low, int upper);

	const QCPBubbleData& data() const { return mData; }
	QCPBubbleData& data() { return mData; }

protected:
	// property members:
	QCPBubbleData mData;

	QCPRange mBubbleSize;

	// reimplemented virtual methods:
	virtual void draw(QCPPainter* painter) Q_DECL_OVERRIDE;
	virtual void drawLegendIcon(QCPPainter* painter, const QRectF& rect) const Q_DECL_OVERRIDE;

	double bubbleSize(double bubbleValue) const;
	void getBubbles(QVector<QVector3D>* bubbles) const;
};