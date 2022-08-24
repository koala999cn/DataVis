#include "QCPBubbleChart.h"


QCPBubbleData::QCPBubbleData()
{

}


QCPBubbleData::QCPBubbleData(int bubbleSize)
{
    mData.resize(bubbleSize);
}


QCPBubbleData::~QCPBubbleData()
{

}


QCPBubbleData::QCPBubbleData(const QCPBubbleData& other) :
    mData(other.mData)
{

}


QCPBubbleData::QCPBubbleData(QCPBubbleData&& other) :
    mData(std::move(other.mData))
{

}


QCPBubbleData& QCPBubbleData::operator=(const QCPBubbleData& other)
{
    if (&other != this)
    {
        mData = other.mData;
        mKeyRange = other.mKeyRange;
        mValueRange = other.mValueRange;
        mBubbleBounds = other.mBubbleBounds;
    }

    return *this;
}


QCPBubbleData& QCPBubbleData::operator=(QCPBubbleData&& other)
{
    if (&other != this)
    {
        mData = std::move(other.mData);
        mKeyRange = other.mKeyRange;
        mValueRange = other.mValueRange;
        mBubbleBounds = other.mBubbleBounds;
    }

    return *this;
}


void QCPBubbleData::setSize(int size)
{
    mData.resize(size);
}


void QCPBubbleData::setData(int index, double key, double value, double z)
{
    mData[index] = QVector3D( key, value, z );

    // TODO: 同步更新各range
}


void QCPBubbleData::recalculateRanges()
{
    if (mData.empty()) return;

    mKeyRange.lower = mKeyRange.upper = mData[0].x(); 
    mValueRange.lower = mValueRange.upper = mData[0].y();
    mBubbleBounds.lower = mBubbleBounds.upper = mData[0].z();

    for (unsigned i = 1; i < mData.size(); i++) {
        if (mData[i].x() < mKeyRange.lower)
            mKeyRange.lower = mData[i].x();
        else if (mData[i].x() > mKeyRange.upper)
            mKeyRange.upper = mData[i].x();

        if (mData[i].y() < mValueRange.lower)
            mValueRange.lower = mData[i].y();
        else if (mData[i].y() > mValueRange.upper)
            mValueRange.upper = mData[i].y();

        if (mData[i].z() < mBubbleBounds.lower)
            mBubbleBounds.lower = mData[i].z();
        else if (mData[i].z() > mBubbleBounds.upper)
            mBubbleBounds.upper = mData[i].z();
    }
}


void QCPBubbleData::clear()
{
    mData.clear();
}


QCPBubbleChart::QCPBubbleChart(QCPAxis* keyAxis, QCPAxis* valueAxis) :
	QCPAbstractPlottable(keyAxis, valueAxis)
{
    mBubbleSize.lower = 3;
    mBubbleSize.upper = 30;
}


QCPBubbleChart::~QCPBubbleChart()
{

}


double QCPBubbleChart::selectTest(const QPointF& pos, bool onlySelectable, QVariant* details) const
{
    // TODO:
    return -1;
}


QCPRange QCPBubbleChart::getKeyRange(bool& foundRange, QCP::SignDomain signDomain) const
{
    if (mData.isEmpty())
    {
        foundRange = false;
        return QCPRange();
    }
    
    foundRange = true;
    return mData.keyRange(); // TODO: 考虑signDomain
}


QCPRange QCPBubbleChart::getValueRange(bool& foundRange, QCP::SignDomain signDomain, const QCPRange& keyRange) const
{
    if (mData.isEmpty())
    {
        foundRange = false;
        return QCPRange();
    }

    foundRange = true;
    return mData.valueRange(); // TODO: 考虑signDomain和keyRange
}


void QCPBubbleChart::setData(const QCPBubbleData& data)
{
    mData = data;
}


void QCPBubbleChart::setData(QCPBubbleData&& data)
{
    std::swap(mData, data);
}


void QCPBubbleChart::getBubbles(QVector<QVector3D>* bubbles) const
{
    if (!bubbles) return;
    QCPAxis* keyAxis = mKeyAxis.data();
    QCPAxis* valueAxis = mValueAxis.data();
    if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; bubbles->clear(); return; }

    bubbles->resize(mData.size());
    if (keyAxis->orientation() == Qt::Vertical)
    {
        for (int i = 0; i < mData.size(); ++i)
        {
            if (!qIsNaN(mData.at(i).z()))
            {
                (*bubbles)[i].setX(valueAxis->coordToPixel(mData.at(i).y()));
                (*bubbles)[i].setY(keyAxis->coordToPixel(mData.at(i).x()));
                (*bubbles)[i].setZ(bubbleSize(mData.at(i).z()));
            }
        }
    }
    else
    {
        for (int i = 0; i < mData.size(); ++i)
        {
            if (!qIsNaN(mData.at(i).z()))
            {
                (*bubbles)[i].setX(keyAxis->coordToPixel(mData.at(i).x()));
                (*bubbles)[i].setY(valueAxis->coordToPixel(mData.at(i).y()));
                (*bubbles)[i].setZ(bubbleSize(mData.at(i).z()));
            }
        }
    }
}


void QCPBubbleChart::setBubbleSize(int low, int upper)
{
    mBubbleSize.lower = low;
    mBubbleSize.upper = upper;
}


double QCPBubbleChart::bubbleSize(double bubbleValue) const
{
    auto bb = mData.bubbleBounds();
    return mBubbleSize.lower + mBubbleSize.size() * (bubbleValue - bb.lower) / bb.size();
}


void QCPBubbleChart::draw(QCPPainter* painter)
{
    if (mData.isEmpty()) return;
    if (!mKeyAxis || !mValueAxis) return;

    painter->setBrush(mBrush);
    painter->setPen(mPen);

    QVector<QVector3D> bubbles;
    getBubbles(&bubbles);
    for(auto& i : bubbles)
        painter->drawEllipse(QPointF(i.x(), i.y()), i.z()/2, i.z()/2);
}


void QCPBubbleChart::drawLegendIcon(QCPPainter* painter, const QRectF& rect) const
{

}