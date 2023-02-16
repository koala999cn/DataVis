#pragma once
#include "KtAABB.h"
#include "KeAlignment.h"


//
// 布局元素抽象类
// 每个布局元素由外边框(outterRect)、内边框(innerRect)和绘制内容(content)构成
// 元素内容的绘制限制在内边框(等于clipRect)，外边框和内边框的间隔由margins确定
// margins由用户配置, contentSize由继承类计算
// outterRect, innerRect由layout引擎算定
// 
// 目前仅支持两种类型的元素：
// 一是固定尺寸，此时calcSize返回非零值，layout引擎尽量按该尺寸为元素分配空间
// 二是浮动尺寸，此时calcSize返回零值，layout引擎将剩余空间按照extraShares返回的份额为元素分配空间
// 

class KvLayoutElement
{
public:
	using float_t = double;
	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	KvLayoutElement() = default;
	KvLayoutElement(KvLayoutElement* parent) : parent_(parent) {}

	KeAlignment align() const { return align_; }
	KeAlignment& align() { return align_; }

	KvLayoutElement* parent() { return parent_; }
	void setParent(KvLayoutElement* p) { parent_ = p; }

	// 调用arrange系列函数之前，须先调用calcSize

	virtual void arrange_(int dim, float_t lower, float_t upper);

	void arrange(const rect_t& rc) {
		for (int i = 0; i < 2; i++)
			arrange_(i, rc.lower()[i], rc.upper()[i]);
	}

	margins_t& margins() { return margins_; }
	const margins_t& margins() const { return margins_; }

	void setMargins(float l, float t, float r, float b);

	const rect_t& outterRect() const { return oRect_; }

	const rect_t& innerRect() const { return iRect_; }

	const size_t& contentSize() const { return contentSize_; }


	// 计算content的尺寸，结果放置在iRect_中
	// iRect_的lower点等于margins_的lower，iRect_的size等于contentSize
	// 返回非0代表固定尺寸，返回0表示可拉伸尺寸，由layout引擎根据可用空间分配
	void calcSize(void* cxt) const { 
		contentSize_ = calcSize_(cxt);
	}

	// 返回期望的空间大小，由内部使用
	// 在calcSize之后，arrange之前调用
	size_t expectRoom() const {	
		size_t room(0); 
		if (contentSize().x() > 0) 
			room.x() = contentSize_.x() + margins_.lower().x() + margins_.upper().x();
		if (contentSize().y() > 0)
			room.y() = contentSize_.y() + margins_.lower().y() + margins_.upper().y();
		return room;
	}

	// 返回x/y维度需要引擎分配空间的份额
	virtual point2i extraShares() const {
		return { contentSize_.x() == 0 ? shareFactor_.x() : 0,
			contentSize_.y() == 0 ? shareFactor_.y()  : 0 };
	}

	const point2i& shareFactor() const { return shareFactor_; }
	void setShareFactor(const point2i& f) { shareFactor_ = f; }

protected:
	virtual size_t calcSize_(void* cxt) const = 0;

protected:
	mutable size_t contentSize_{ 0 };
	rect_t iRect_{ point_t(0) ,point_t(0) }, oRect_{ point_t(0) ,point_t(0) }; // 内、外边框。外边框用于布局，内边框用于绘制
	margins_t margins_{ point_t(0) ,point_t(0) }; // 内外边框间的留白，缺省无留白
	KvLayoutElement* parent_{ nullptr };
	point2i shareFactor_{ 1, 1 }; // 份额因子，值越大分配的空间越多
	KeAlignment align_{ 0 }; // 对齐方式。当分配的空间大于contentSize时，该标记决定设置iRect的方式
};
