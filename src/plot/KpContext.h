#pragma once
#include "KtColor.h"


// 一些绘制元素的上下文属性


class KpLineContext
{
public:

	enum KeType
	{
		k_line_none,
		k_line_solid,
		k_line_dash,
		k_line_dot
	};

	int type;
	float width;
	color4f color;

	KpLineContext() : type(k_line_solid), width(1), color(0, 0, 0, 1) {

	}
};


class KpFillContext
{
public:

	enum KeType
	{
		k_fill_none,
		k_fill_solid
	};

	int type;
	color4f color;
};


class KpScatterContext
{
public:

	enum KeType
	{
		k_line_point
	};

	int type;
	float size;
	color4f color;
	KpLineContext stroke;
};
