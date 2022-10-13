#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtPoint.h"

class KcAxis;

// ��ά����ϵʵ��

class KcCoord2d : public KvRenderable
{
public:
	using float_type = typename KvRenderable::float_type;
	using point2 = KtPoint<float_type, 2>;

	enum KeAxis 
	{
		k_left,
		k_right,
		k_top,
		k_bottom
	};

	KcCoord2d();
	KcCoord2d(const point2& lower, const point2& upper);

	void setExtents(const point2& lower, const point2& upper);

	point2 lower() const; // the lower conner
	point2 upper() const; // the upper conner

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axis(KeAxis id) const { return axes_[id]; }

	// ������ϵAABB�����ĵ�Ϊ��׼���Ը��������extent/range���еȱ�������
	// ������ϵ����ǰ��AABB�����ĵ㱣�ֲ���
	// factor=1ʱ������ϵ������
	// factor=0ʱ������ϵ���������ĵ�
	void zoom(double factor);

	void draw(KvPaint*) const override;

private:
	axis_ptr axes_[4];
};
