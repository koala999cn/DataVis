#pragma once
#include "KcColorMap.h"


// ����ͼ��������2d/3d����
//   - ����2d���ݣ�y��Ϊ������Ϊ����ֵ
//   - ����3d���ݣ�yΪ���꣬zΪ����ֵ

class KcBubble2d : public KcColorMap
{
	using super_ = KcColorMap;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	float mapValueToSize_(float_t val) const;

private:

	// ����value�Ĵ�С����bubble�Ĵ�С���в�ֵ
	bool radiusAsSize_{ false }; // true��ʾ���뾶��ֵ�����������ֵ
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // ��ֵ��Χ

	bool enableSizeInterp_{ true }; // ���ô�С��ֵ
 	bool enableColorInterp_{ false }; // ������ɫ��ֵ
};
