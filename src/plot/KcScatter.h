#pragma once
#include "KvPlottable1d.h"
#include "KmLabeling.h"
#include "KpContext.h"


// ɢ��ͼ

class KcScatter : public KvPlottable1d, public KmLabeling
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	void setData(const_data_ptr d) override;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	const KpMarker& marker() const { return marker_; }
	void setMarker(const KpMarker& m) { marker_ = m; }

	bool sizeVaryingByArea() const { return varyingByArea_; }
	void setSizeVaryingByArea(bool b);

	float sizeLower() const { return sizeLower_; }
	void setSizeLower(float s);

	float sizeUpper() const { return sizeUpper_; }
	void setSizeUpper(float s);

	bool sizeVarying() const { return sizeVarying_; }
	void setSizeVarying(bool b);

	unsigned sizeVaryingDim() const { return dimSizeVarying_; }
	void setSizeVaryingDim(unsigned d);

private:

	bool objectVisible_(unsigned objIdx) const override;

	bool objectReusable_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	float mapValueToSize_(float_t val) const;

	KuDataUtil::KpPointGetter1d pointsAt_(unsigned ch) const;

	void* drawMarker_(KvPaint*, unsigned) const;
	void* drawLabel_(KvPaint*) const;

protected:
	KpMarker marker_;

	// ����value�Ĵ�С����bubble�ĳߴ���в�ֵ
	bool sizeVarying_{ false };
	unsigned dimSizeVarying_{ 1 };
	bool varyingByArea_{ false }; // true��ʾ�������ֵ�����򰴳��Ȳ�ֵ
	float sizeLower_{ 3 }, sizeUpper_{ 33 }; // �ߴ��ֵ��Χ
};
