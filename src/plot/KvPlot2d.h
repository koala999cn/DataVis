#pragma once
#include "KvPlot.h"
#include "KcCoord2d.h"
#include "KvRenderable.h"


// 二维plot的抽象类

class KvPlot2d
{
public:
	using axis_ptr = std::shared_ptr<KcAxis>;
	using point2 = point2d;


protected:
	std::unique_ptr<KcCoord2d> coord_; // 内置创建并管理
};
