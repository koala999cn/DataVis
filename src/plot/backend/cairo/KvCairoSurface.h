#pragma once
#include "KtAABB.h"
#include "plot/KtColor.h"

// 对cairo-surface的封装

class KvCairoSurface
{
	using rect_t = KtAABB<double, 2>;

public:
	KvCairoSurface() = default;
	virtual ~KvCairoSurface();

	bool create(const rect_t& rc); // 创建cairo-surface，返回true表示重新创建了surface，否则表示重用已有的surface
	void destroy(); // 销毁cairo绘制表面
	void clear(const color4f& clr); // 用clr填充cairo绘制表面/绘制背景

	void setColor(const color4f& clr); // 设置当前主色
	void setClipRect(const rect_t& rc); // 设置剪切框

	auto width() const { return canvas_.width(); }
	auto height() const { return canvas_.height(); }

	void* cr() const { return cxt_; }
	void* surface() const { return surf_; }
	const rect_t& canvas() const { return canvas_; }

protected:
	virtual void* createSurface_() const = 0; // 子类须重载该方法，以创建不同的绘制表面（image，pdf，ps，svg等）

protected:
	void* cxt_{ nullptr }; // the cr of cairo
	void* surf_{ nullptr }; // the surface of cairo
	rect_t canvas_;
};