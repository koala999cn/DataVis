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
//    五是默认z坐标管理
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

	bool forceDefaultZ() const { return forceDefaultZ_; }
	bool& forceDefaultZ() { return forceDefaultZ_; }

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

	float brightenCoeff() const { return brightenCoeff_; }
	float& brightenCoeff() { return brightenCoeff_; }

	auto& colorMappingRange() const { return colorMappingRange_; }
	auto& colorMappingRange() { return colorMappingRange_; }

	unsigned colorMappingDim() const { return colorMappingDim_; }
	void setColorMappingDim(unsigned d); // 该值很关键，使用set赋值

	void resetColorMappingRange();

protected:

	// 确保传入的valp为数据原值，而非强制替换z之后的值
	color4f mapValueToColor_(float_t* valp, unsigned channel) const;

	// 根据当前的coloringMode_配置主色
	void updateColorMappingPalette_();

	// 返回一个离散化的数据对象
	// 如果data()成员本身为离散数据，则直接返回；否则按照sampCount_构建并返回一个采样对象
	std::shared_ptr<KvDiscreted> discreted_() const;

	// 是否正在是否defaultZ，满足2个条件之一为true：
	// 一是data_的dim等于1，这种情况始终使用defaultZ填补缺失的z值；
	// 二是forceDefaultZ_为真
	bool usingDefaultZ_() const;

	// 根据valp构建point3对象
	// 如果forceDefaultZ_为真，则替换valp的z值，否则用原z值
	point3 toPoint_(float_t* valp, unsigned ch) const {
		return { valp[0], valp[1], forceDefaultZ() ? defaultZ(ch) : valp[2] };
	}

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	// 绘制数据
	data_ptr data_;

	// 色彩管理
	KeColoringMode coloringMode_{ k_one_color_solid };
	KtGradient<float_t, color4f> colorBar_; // 色带
	bool flatShading_{ false };  // 若true，则开启flat渲染模式，否则使用smooth渲染模式
	                             // flat模式下，使用多边形的最后1个顶点对整个多边形着色

	float brightenCoeff_{ 0.5 }; // 亮度增强系数，仅适用于k_one_color_gradiant色彩模式
	unsigned colorMappingDim_{ 2 }; // 使用该维度的数据进行色彩映射
	std::pair<float_t, float_t> colorMappingRange_; // 色彩映射的值域范围

	// 各维度的采样点数目, 仅适用于连续数据
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	// 用于分离坐标轴，缺省为null，表示使用主坐标轴
	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; 

	// 对Z轴的特殊处理：有2个作用：
	// 一是对于一维数据，填补缺省的z值，以便在3d空间绘制
	// 二是对于高维数据，替换原本的z值，以便在3d空间投影到x-y平面绘制

	float_t defaultZ_{ 0 }; // 二维数据的z轴将被置为该值
	float_t stepZ_{ 1 }; // 多通道二维数据的z轴偏移。若须将多通道数据显示在一个z平面，置该值为0

	// 该标记为真时，将强制用默认Z值替换原来的z值，可用来在3d空间绘制二维的colormap图
	bool forceDefaultZ_{ false };
};
