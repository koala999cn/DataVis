#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"


// 序列图的基类，主要处理串行数据，用于绘制折线图graph、散点图scatter等
// 兼容2d和3d模式，抽象stacked绘图模式

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

protected:

	unsigned objectCount() const override;

	// 绘制第objIdx个渲染对象，并返回可复用的对象id
	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	// 每个批次的渲染对象数目，有的实现可能fill和edge分别有1个对象，有的可能text还有1个对象
	// 通常每个通道对应1个批次
	virtual unsigned objectsPerBatch_() const { return 1; }

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 为了兼容连续数据，此处增加unsigned参数，表示数据count
	// GETTER不作z替换（数据dim>1时），但完成z附加（数据dim=1时），确保返回的数据尺寸 >= 3
	// 继承类须根据forceDefaultZ对GETTER返回值作二次处理（自行完成z值替换，或其他操作）
	virtual void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const = 0;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;

	// 将渲染对象id转换为通道号
	unsigned objIdx2ChsIdx_(unsigned objIdx) const;

	// 返回每个通道包含的1d数据数目. 对于samp2d数据，返回samp2d::size(0)
	unsigned linesPerChannel_() const {
		return KuDataUtil::pointGetter1dCount(discreted_());
	}

	// data1d数据总数
	unsigned linesTotal_() const;

	// 返回第ch通道的第idx条1d数据访问接口
	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const;

	aabb_t calcBoundingBox_() const override;

	////////////////////////////////////////////////////////////////////////
	// 
	// stack绘制模式支持相关接口

	enum KeStackMode
	{
		k_stack_none, // 无堆叠
		k_stack_channel, // 按通道堆叠
		k_stack_column // 按列（x轴向）堆叠（仅对二维采样数据）
	};

	void setStackMode_(int mode);

private:

	void calcStackData_() const; // 计算stack数据，内部调用

	KuDataUtil::KpPointGetter1d lineStack_(unsigned ch, unsigned idx) const;

	int stackMode_{ k_stack_none };

	mutable std::vector<std::vector<float_t>> stackedData_; // 保存stack数据计算结果

	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////
	// 
	// ridge绘制模式支持相关接口

protected:

	enum KeRidgeMode
	{
		k_ridge_none,
		k_ridge_channel,
		k_ridge_column,
		k_ridge_all
	};

	void setRidgeMode_(int mode);

	float_t ridgeOffsetAt_(unsigned ch, unsigned idx) const;

public:

	float_t ridgeOffset() const { return ridgeOffset_; }
	void setRidgeOffset(float_t offset);

private:
	int ridgeMode_{ k_ridge_none };
	float_t ridgeOffset_{ 1.0 };

	////////////////////////////////////////////////////////////////////////

private:

	// 一维数据(x, y)在x-y平面绘图（使用缺省z值）
	void* draw1d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// 二维数据(x, y, z)画瀑布图, 以x为流动轴
	void* draw2d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// 二维数据(x, y, z)画3d图
	void* draw3d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

};
