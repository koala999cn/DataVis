#pragma once
#include <string>
#include "KtColor.h"


// 一些绘制元素的上下文属性


class KpPen
{
public:

	enum KeStyle
	{
		k_none,
		k_solid,
		k_dot,
		k_dash,
		k_dash4,
		k_dash8,
		k_dash_dot,
		k_dash_dot_dot
	};

	int style{ k_solid };
	float width{ 1 };
	color4f color{ 0, 0, 0, 1 };

	static const KpPen& null() {
		static KpPen nullPen{ k_none, 0, color4f(0) };
		return nullPen;
	}

	bool visible() const {
		return style != k_none && color.a() != 0;
	}
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
	color4f color{ 1, 1, 1, 1 };

	static const KpBrush& null() {
		static KpBrush nullBrush{ k_none, color4f(0) };
		return nullBrush;
	}

	bool visible() const {
		return style != k_none && color.a() != 0;
	}
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
