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

	// 12���������id���壬����������axes_
	enum KeAxis
	{
		k_x,
		k_y,
		k_z,
		k_x_top,
		k_x_near,
		k_x_top_near,
		k_y_right,
		k_y_near,
		k_y_right_near,
		k_z_right,
		k_z_top,
		k_z_right_top
	};

	axis_ptr& axisX() { return axes_[0]; }
	axis_ptr& axisY() { return axes_[1]; }
	axis_ptr& axisZ() { return axes_[2]; }

	axis_ptr axisX() const { return axes_[0]; }
	axis_ptr axisY() const { return axes_[1]; }
	axis_ptr axisZ() const { return axes_[2]; }

private:
	QColor bkgnd_;
	bool visible_;

	std::vector<axis_ptr> axes_;
};
