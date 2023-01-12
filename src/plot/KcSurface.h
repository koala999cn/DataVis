#pragma once
#include "KvPlottable2d.h"


// ����grid(sampled2d)���ݵ�quads����

class KcSurface : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:
	using super_::super_;

private:
	void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const final;

private:
	bool flat_{ false }; // �Ƿ�ʹ��flatģʽ��Ⱦ. ��true��ÿ��quadʹ��ͬɫ��Ⱦ
	unsigned flatIdx_{ 0 }; // flatģʽ�£�������Ⱦquad�Ķ������
	                        // ��������ÿ��quad��ʹ�ø�quad�ĵ�flatIdx_�������ɫ��Ⱦ����quad
};
