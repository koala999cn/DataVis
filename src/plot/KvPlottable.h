#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KtGradient.h"
#include "KcAxis.h"

class KvData;
class KvDiscreted;

//
// 可绘制对象的基类. 内置实现以下功能：
//    一是绘制数据的存储
//    二是色彩模式和主色、辅色配置
//    三是对连续数据的采样，派生类只须绘制离散数据
//    四是分离坐标轴的设置和存储
//    五是二维数据的缺省z坐标配置
//

class KvPlottable : public KvRenderable
{
	using super_ = KvRenderable;

public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	KvPlottable(const std::string_view& name);

	bool empty() const; // 若无数据，或数据为空，返回true

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

	/// 每个plottable均配置若干主色和1个辅色
    /// 主色的管理由内置colorBar_对象实现，辅色的管理须由用户重载实现

	// 返回此plottable需要的主色数目，返回-1表示需要连续色带
	// 提供缺省实现：当KeColoringMode为k_coloring_flat_shading或k_coloring_smooth_shading时，返回-1
	// 否则返回数据的通道数目，即每个通道配置1个主色
	virtual unsigned majorColorsNeeded() const;

	// 设置主色
	// 提供缺省实现，将majors对象存储为colorBar_成员的控制色
	// 用户可重载该函数，以同步存储在其他地方（如pen，brush等上下文）的色彩值
	virtual void setMajorColors(const std::vector<color4f>& majors);

	// 主色彩的个数，一般等于majorColorNeeded。
	// 在连续色彩情况下，返回主控制色的个数
	// 提供缺省实现，返回colorBar_的控制色数目
	virtual unsigned majorColors() const;

	// 返回第idx个主色
	// assert(idx < majorColors())
	// 提供缺省实现，返回colorBar_的第idx个控制色
	virtual color4f majorColor(unsigned idx) const;


	// 辅色存储和配置由用户实现
	virtual const color4f& minorColor() const = 0;
	virtual void setMinorColor(const color4f& minor) = 0;

	////////////////////////////////////////////////////////////////

	float_t defaultZ() const { return defaultZ_; }
	float_t& defaultZ() { return defaultZ_; }

	float_t stepZ() const { return stepZ_; }
	float_t& stepZ() { return stepZ_; }

	float_t defaultZ(unsigned ch) const { return defaultZ_ + ch * stepZ_; }

	////////////////////////////////////////////////////////////////

	// 色彩模式支持

	enum KeColoringMode
	{
		k_one_color_solid, // 单色
		k_one_color_gradiant, // 单色亮度渐变
		k_two_color_gradiant, // 双色（主色和辅色）渐变

		// 以下模式majorColorsNeeded返回-1，表示需要色带支持
		// 此种模式下，各通道共用1个色带
		k_colorbar_gradiant, // 多色渐变
	};

	KeColoringMode coloringMode() const { return coloringMode_; }
	void setColoringMode(KeColoringMode mode);

	bool flatShading() const { return flatShading_; }
	bool& flatShading() { return flatShading_; }

	auto& colorMappingRange() const { return colorMappingRange_; }
	auto& colorMappingRange() { return colorMappingRange_; }

	void fitColorMappingRange();

protected:

	color4f mapValueToColor_(float_t val, unsigned channel) const;

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	// 绘制数据
	data_ptr data_;

	// 色彩管理
	KeColoringMode coloringMode_{ k_one_color_solid };
	KtGradient<float_t, color4f> colorBar_; // 色带
	std::pair<float_t, float_t> colorMappingRange_; // 色彩映射的值域范围
	bool flatShading_{ false };  // 若true，则开启flat渲染模式，否则使用smooth渲染模式
	                             // flat模式下，使用多边形的第1个顶点对整个多边形着色

	// 各维度的采样点数目, 仅适用于连续数据
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	// 用于分离坐标轴，缺省为null，表示使用主坐标轴
	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; 

	// 以下成员仅对二维数据有效
	float_t defaultZ_{ 0 }; // 二维数据的z轴将被置为该值
	float_t stepZ_{ 1 }; // 多通道二维数据的z轴偏移。若须将多通道数据显示在一个z平面，置该值为0
};
