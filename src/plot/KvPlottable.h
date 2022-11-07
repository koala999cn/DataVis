#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>
#include "KtColor.h"

class KvData;
class KvDiscreted;

class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	data_ptr data() const { return data_; }
	void setData(data_ptr d);

	bool empty() const;

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	unsigned& sampCount(unsigned dim) { return sampCount_[dim]; }

	aabb_type boundingBox() const override;

	// 封装连续数据的绘制，提供另外一个绘制离散数据的接口drawDiscreted_
	void draw(KvPaint*) const override;


	/// 以下为调色板通用接口 //////////////////////////////////

	// 返回-1表示需要连续色带
	virtual unsigned majorColorsNeeded() const = 0;

	// 返回false表示不需要辅助色
	virtual bool minorColorNeeded() const = 0;

	// 主色彩的个数，一般等于majorColorNeeded。
	// 在连续色彩情况下，返回主控制色的个数
	virtual unsigned majorColors() const = 0;

	virtual color4f majorColor(unsigned idx) const = 0;

	virtual void setMajorColors(const std::vector<color4f>& majors) = 0;

	virtual color4f minorColor() const = 0;

	virtual void setMinorColor(const color4f& minor) = 0;

	////////////////////////////////////////////////////////////////

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	data_ptr data_;

	// 各维度的采样点数目, 仅适用于连续数据
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 
};
