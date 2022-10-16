#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include "KtPoint.h"

class KcAxis;
class KcGridPlane;

// ��ά����ϵʵ�֣���12���������6��ƽ�湹��
// Ĭ��X�����ң�Y�����ϣ�Z������

class KcCoord3d : public KvRenderable
{
public:
	using point3 = KtPoint<float_t, 3>;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using grid_plane_ptr = std::shared_ptr<KcGridPlane>;
	

	// 12���������id����������axes_
	enum KeAxis
	{
		k_x0, /*bottom-far*/	k_y0, /*left-far*/		k_z0, /*left-bottom*/

		k_x1, /*top-far*/		k_y1, /*right-far*/		k_z1, /*right-bottom*/

		k_x2, /*top-near*/		k_y2, /*right-near*/	k_z2, /*right-top*/

		k_x3, /*bottom_near*/	k_y3, /*left_near*/		k_z3  /*left_top*/
	};

	// 6�����Ի���grid��ƽ��
	enum KeGrid
	{
		/*
		k_back_h, k_y0_y1 = k_back_h,
		k_back_v, k_x0_x1 = k_back_v,

		k_front_h, k_y2_y3 = k_front_h,
		k_front_v, k_x2_x3 = k_front_v,*/

		k_xy_back,
		//k_x0_y0 = k_back, k_x0_y1 = k_back, k_x1_y0 = k_back, k_x1_y1 = k_back,

		k_xy_front, 
		//k_x2_y2 = k_front, k_x2_y3 = k_front, k_x3_y2 = k_front, k_x3_y3 = k_front,

		k_yz_left, 
		//k_y0_z0 = k_left, k_y0_z3 = k_left, k_y3_z0 = k_left, k_y3_z3 = k_left,

		k_yz_right,
		//k_y1_z1 = k_right, k_y1_z2 = k_right, k_y2_z1 = k_right, k_y2_z2 = k_right,

		k_xz_ceil,
		//k_x1_z2 = k_ceil, k_x1_z3 = k_ceil, k_x2_z2 = k_ceil, k_x2_z3 = k_ceil,

		k_xz_floor,
		//k_x0_z0 = k_floor, k_x0_z1 = k_floor, k_x3_z0 = k_floor, k_x3_z1 = k_floor
	};

public:

	KcCoord3d();
	KcCoord3d(const point3& lower, const point3& upper);

	void setExtents(const point3& lower, const point3& upper);

	point3 lower() const; // the lower conner
	point3 upper() const; // the upper conner

	point3 center() const; // the center point

	// ����aabb�ĶԽ��߳���
	double diag() const;

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axis(KeAxis id) const { return axes_[id]; }

	// ������ϵAABB�����ĵ�Ϊ��׼���Ը��������extent/range���еȱ�������
	// ������ϵ����ǰ��AABB�����ĵ㱣�ֲ���
	// factor=1ʱ������ϵ������
	// factor=0ʱ������ϵ���������ĵ�
	void zoom(double factor);

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;

private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
};