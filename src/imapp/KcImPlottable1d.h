#pragma once
#include "plot/KvPlottable.h"


class KcImPlottable1d : public KvPlottable
{
public:

	// ֧�ֵ�6��һάͼ����. TODO: ������͵�֧��
	enum KeType
	{
		k_scatter,  // ɢ��ͼ
		k_line,   // ����ͼ
		k_line_scatter, // ����ͼ
		k_line_fill, // ���ͼ
		k_bars,   // �����״ͼ
		k_bars_grouped, // ������״ͼ
		k_type_count
	};

	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;

	int type() const { return type_; }
	int& type() { return type_; }

private:
	int type_{ k_line };
};
