#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// ����ͼ����������ͼgraph��ɢ��ͼscatter����״ͼbar��
// ����2d��3dģʽ

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;

protected:

	using point_getter = typename KvPaint::point_getter;

	// Ϊ�˼����������ݣ��˴�����unsigned��������ʾ����count
	virtual void drawImpl_(KvPaint*, point_getter, unsigned, const color4f&) const = 0;

private:

	// ���³�Ա���Զ�ά������Ч

	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0
};
