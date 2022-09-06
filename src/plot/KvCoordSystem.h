#pragma once
#include <vector>
#include <memory>
#include <QColor>

class KvAxis;

// ����ϵ����
// Ĭ��X�����ң�Y�����ϣ�Z������

class KvCoordSystem
{
	using axis_ptr = std::shared_ptr<KvAxis>;

public:

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

private:
	QColor bkgnd_;
	bool visible_;

	std::vector<axis_ptr> axes_;
};
