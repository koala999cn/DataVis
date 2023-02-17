#pragma once
#include "KvPlottable1d.h"


// ����ͼ��������2d/3d����. �������ǳߴ��ֵ
// 
//   - ����2d���ݣ�y��Ϊ������Ϊ����ֵ
//   - ����3d���ݣ�yΪ���꣬zΪ����ֵ
// 

class KcBubble : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;
	void setMinorColor_(const color4f& minor) override;

	bool radiusAsSize() const { return radiusAsSize_; }
	bool& radiusAsSize() { return radiusAsSize_; }

	float sizeLower() const { return sizeLower_; }
	float& sizeLower() { return sizeLower_; }

	float sizeUpper() const { return sizeUpper_; }
	float& sizeUpper() { return sizeUpper_; }

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

protected:

	bool showFill_() const final;

	bool showEdge_() const final;

	void setRenderState_(KvPaint*, unsigned objIdx) const final;

	void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const final;

	float mapValueToSize_(float_t val) const;

private:

	// ����value�Ĵ�С����bubble�ĳߴ���в�ֵ
	bool radiusAsSize_{ false }; // true��ʾ���뾶��ֵ�����������ֵ
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // �ߴ��ֵ��Χ

	color4f clrMinor_{ 0, 0, 0, 1 }; // ��ɫ�����ڻ��ƽ���ɫ

	bool showText_{ true };
	color4f clrText_{ 1, 0, 0, 1 };
};
