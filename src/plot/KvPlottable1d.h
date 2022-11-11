#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// ����ͼ�Ļ��࣬��������ͼgraph��ɢ��ͼscatter����״ͼbar��
// ����2d��3dģʽ

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter = typename KvPaint::point_getter;

	// Ϊ�˼����������ݣ��˴�����unsigned��������ʾ����count
	virtual void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const = 0;

private:

	// һά����(x, y)��x-yƽ���ͼ��ʹ��ȱʡzֵ��
	void draw1d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)���ٲ�ͼ, ��xΪ������
	void draw2d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)��3dͼ
	void draw3d_(KvPaint*, KvDiscreted*) const;

private:

	// ���³�Ա���Զ�ά������Ч

	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0
};
