#pragma once
#include "plot/KvGraph.h"


class KcImGraph2d : public KvGraph
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

	using KvGraph::KvGraph;

	void draw(KvPaint*) const final;
};
