#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


class KvPlottable3d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

	void draw(KvPaint*) const override;

protected:

	using point_getter = typename KvPaint::point_getter;
	virtual void drawImpl_(KvPaint*, point_getter, const color4f&) const = 0;

private:
	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0
};
