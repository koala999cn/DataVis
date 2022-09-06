#pragma once
#include <vector>
#include <memory>
#include <QColor>

class KvAxis;

// 坐标系基类
// 默认X轴向右，Y轴向上，Z轴向外

class KvCoordSystem
{
	using axis_ptr = std::shared_ptr<KvAxis>;

public:

	// 12根坐标轴的id定义，可用于索引axes_
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
