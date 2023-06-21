#pragma once
#include "KtAABB.h"
#include "plot/KtColor.h"

// ��cairo-surface�ķ�װ

class KvCairoSurface
{
	using rect_t = KtAABB<double, 2>;

public:
	KvCairoSurface() = default;
	virtual ~KvCairoSurface();

	bool create(const rect_t& rc); // ����cairo-surface������true��ʾ���´�����surface�������ʾ�������е�surface
	void destroy(); // ����cairo���Ʊ���
	void clear(const color4f& clr); // ��clr���cairo���Ʊ���/���Ʊ���

	void setColor(const color4f& clr); // ���õ�ǰ��ɫ
	void setClipRect(const rect_t& rc); // ���ü��п�

	auto width() const { return canvas_.width(); }
	auto height() const { return canvas_.height(); }

	void* cr() const { return cxt_; }
	void* surface() const { return surf_; }
	const rect_t& canvas() const { return canvas_; }

protected:
	virtual void* createSurface_() const = 0; // ���������ظ÷������Դ�����ͬ�Ļ��Ʊ��棨image��pdf��ps��svg�ȣ�

protected:
	void* cxt_{ nullptr }; // the cr of cairo
	void* surf_{ nullptr }; // the surface of cairo
	rect_t canvas_;
};