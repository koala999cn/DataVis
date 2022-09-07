#pragma once
#include "KvRenderable.h"
#include <vector>
#include <memory>
#include <QColor>

class KvAxis;

// ����ϵ����
// Ĭ��X�����ң�Y�����ϣ�Z������

class KvCoordSystem : public KvRenderable
{
	using axis_ptr = std::shared_ptr<KvAxis>;

public:

	KvCoordSystem() : axes_(12, nullptr) {}

	// 12���������id����������axes_
	enum KeAxis
	{
		k_x0, // bottom-far
		k_y0, // left-far
		k_z0, // left-bottom

		k_x1, // top-far,
		k_y1, // right-far,
		k_z1, // right-bottom,

		k_x2, // top-near,
		k_y2, // right-near,
		k_z2, // right-top,

		k_x3, // bottom_near,
		k_y3, // left_near,
		k_z3  // left_top
	};

	axis_ptr& axis(KeAxis id) { return axes_[id]; }
	axis_ptr axisX(KeAxis id) const { return axes_[id]; }

private:
	QColor bkgnd_;
	bool visible_;

	std::vector<axis_ptr> axes_;
};
