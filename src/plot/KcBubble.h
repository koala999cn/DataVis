#pragma once
#include "KcScatter.h"


// ����ͼ��������2d/3d����. �������ǳߴ��ֵ
// 
//   - ����2d���ݣ�y��Ϊ������Ϊ����ֵ
//   - ����3d���ݣ�yΪ���꣬zΪ����ֵ
// 

class KcBubble : public KcScatter
{
	using super_ = KcScatter;

public:

	using super_::super_;

protected:
	using point_getter1 = typename KvPlottable1d::point_getter1;
	virtual void drawImpl_(KvPaint*, point_getter1, unsigned count, unsigned channels) const final;

	float mapValueToSize_(float_t val) const;

private:

	// ����value�Ĵ�С����bubble�ĳߴ���в�ֵ
	bool radiusAsSize_{ false }; // true��ʾ���뾶��ֵ�����������ֵ
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // �ߴ��ֵ��Χ
};
