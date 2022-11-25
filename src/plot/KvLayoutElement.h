#pragma once
#include "KtAABB.h"
#include "KtMargins.h"


//
// 布局元素抽象类
// 每个布局元素由外边框(outterRect)、内边框(innerRect)和绘制内容(content)构成
// 元素内容的绘制限制在内边框(等于clipRect)，外边框和内边框的间隔由margins确定
// margins由用户配置
// outterRect, innerRect由layout引擎协商设定
// contentSize由继承类计算
//

class KvLayoutElement
{
	using float_t = double;

public:

	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	constexpr static float_t null_size{ 0 };
	constexpr static float_t auto_fit_size{ -1 };

	
	void setMargins(const margins_t& m) { margins_ = m; }
	const margins_t& margins() const { return margins_; }

	
	virtual void setOutterRect(const rect_t& rc) {
		oRect_ = iRect_ = rc;
		iRect_.shrink(margins_);
	}

	rect_t outterRect() const { return oRect_; }

	rect_t innerRect() const { return iRect_; }

	
	size_t calcContentSize() const {
		return contentSize_ = calcContentSize_();
	}

	const size_t& contentSize() const { return contentSize_; }

	// 返回期望的空间大小
	virtual size_t expectRoom() const {
		return contentSize_ + margins_.lower() + margins_.upper();
	}

protected:
	virtual size_t calcContentSize_() const = 0;

private:
	mutable size_t contentSize_; // 缓存的元素内容尺寸
	rect_t iRect_, oRect_; // 内、外边框
	margins_t margins_{ point_t(0) ,point_t(0) }; // 内外边框间的留白，缺省无留白
};
