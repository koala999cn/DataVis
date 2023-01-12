#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// ����ͼ�Ļ��࣬��Ҫ���������ݣ����ڻ�������ͼgraph��ɢ��ͼscatter����״ͼbar��
// ����2d��3dģʽ

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter1 = typename KvPaint::point_getter1;

	// Ϊ�˼����������ݣ��˴�����unsigned��������ʾ����count
	virtual void drawImpl_(KvPaint*, point_getter1, unsigned count, unsigned channels) const = 0;

private:

	// һά����(x, y)��x-yƽ���ͼ��ʹ��ȱʡzֵ��
	void draw1d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)���ٲ�ͼ, ��xΪ������
	void draw2d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)��3dͼ
	void draw3d_(KvPaint*, KvDiscreted*) const;

};
