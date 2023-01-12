#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KcAxis.h"

class KvData;
class KvDiscreted;

// 可绘制对象的抽象类
// 内置实现对连续数据的采样，派生类只须绘制离散数据

class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	bool empty() const;

	data_ptr data() const { return data_; }
	void setData(data_ptr d);

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	unsigned& sampCount(unsigned dim) { return sampCount_[dim]; }

	// 设置第dim维度的分离坐标轴，该函数接管axis的控制权
	void setAxis(unsigned dim, KcAxis* axis) { selfAxes_[dim].reset(axis); }

	// 返回dim维度的分离坐标轴引用
	KcAxis* axis(unsigned dim) const { return selfAxes_[dim].get(); }

	// 该plt是否含有分离坐标轴
	bool hasSelfAxis() const;

	aabb_t boundingBox() const override;

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

	float_t defaultZ() const { return defaultZ_; }
	float_t& defaultZ() { return defaultZ_; }

	float_t stepZ() const { return stepZ_; }
	float_t& stepZ() { return stepZ_; }

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	data_ptr data_;

	// 各维度的采样点数目, 仅适用于连续数据
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; // 用于分离坐标轴，缺省为null，表示使用主坐标轴

	// 以下成员仅对二维数据有效

	float_t defaultZ_{ 0 }; // 二维数据的z轴将被置为该值
	float_t stepZ_{ 1 }; // 多通道二维数据的z轴偏移。若须将多通道数据显示在一个z平面，置该值为0
};
