#pragma once
#include "KtAABB.h"


//
// 布局元素抽象类
// 每个布局元素由外边框(outterRect)、内边框(innerRect)和绘制内容(content)构成
// 元素内容的绘制限制在内边框(等于clipRect)，外边框和内边框的间隔由margins确定
// margins由用户配置
// outterRect, innerRect由layout引擎算定
// 
// 目前仅支持两种类型的元素：
// 一是固定尺寸，此时calcSize返回非零值，layout引擎尽量按该尺寸为元素分配空间
// 二是浮动尺寸，此时calcSize返回零值，layout引擎将剩余空间按照extraShares返回的份额为元素分配空间
// 

class KvLayoutElement
{
	using float_t = double;

public:

	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	KvLayoutElement() = default;
	KvLayoutElement(KvLayoutElement* parent) : parent_(parent) {}

	KvLayoutElement* parent() { return parent_; }
	void setParent(KvLayoutElement* p) { parent_ = p; }

	// 调用该函数之前，须先调用calcSize
	virtual void arrange(const rect_t& rc) {
		if (rc.width()) arrange_(rc, 0);
		if (rc.height()) arrange_(rc, 1);
	}

	void setMargins(const margins_t& m) { margins_ = m; }
	const margins_t& margins() const { return margins_; }

	rect_t outterRect() const { return oRect_; }

	rect_t innerRect() const { return iRect_; }

	// 计算content的尺寸，结果放置在iRect_中
	// iRect_的lower点等于margins_的lower，iRect_的size等于contentSize
	// 返回非0代表固定尺寸，返回0表示可拉伸尺寸，由layout引擎根据可用空间分配
	void calcSize() { 
		iRect_.lower() = margins_.lower();
		iRect_.upper() = iRect_.lower() + calcSize_();
	}

	// 返回期望的空间大小，由内部使用
	// 在calcSize之后，arrange之前调用
	size_t expectRoom() const {	
		auto room = iRect_.upper() + margins_.upper(); // iRect_.upper等于margins_.lower + contentSize
		if (iRect_.width() == 0) room.x() = 0;
		if (iRect_.height() == 0) room.y() = 0;
		return room;
	}

	// 返回x/y维度需要引擎分配空间的份额
	virtual point2i extraShares() const {
		return { iRect_.width() == 0 ? shareFactor_.x() : 0, 
			iRect_.height() == 0 ? shareFactor_.y()  : 0 };
	}

	const point2i& shareFactor() const { return shareFactor_; }
	void setShareFactor(const point2i& f) { shareFactor_ = f; }

protected:
	virtual size_t calcSize_() const = 0;

	// 返回iRect_缓存的contentSize
	// 调用此函数之后，iRect_的尺寸将不再等于contentSize
	float_t arrange_(const rect_t& rc, int dim);

protected:
	rect_t iRect_, oRect_; // 内、外边框。外边框用于布局，内边框用于绘制
	margins_t margins_{ point_t(0) ,point_t(0) }; // 内外边框间的留白，缺省无留白
	KvLayoutElement* parent_{ nullptr };
	point2i shareFactor_{ 1, 1 }; // 份额因子，值越大分配的空间越多
};
