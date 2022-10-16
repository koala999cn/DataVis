#pragma once
#include "KtColor.h"


// 一些绘制元素的上下文属性


class KpPen
{
public:

	enum KeStyle
	{
		k_none,
		k_solid,
		k_dash,
		k_dot,
		k_dash_dot,
		k_dash_dot_dot
	};

	int style{ k_solid };
	float width{ 1 };
	color4f color{ 0, 0, 0, 1 };
};


class KpBrush
{
public:

	enum KeStyle
	{
		k_none,
		k_solid
	};

	int style{ k_solid };
	color4f color{ 0, 0, 0, 1 };
};


class KpFont
{
public:
	std::string family;
	double size{ 13 };
	bool bold{ false };
	bool italic{ false };
	bool underline{ false };
};
