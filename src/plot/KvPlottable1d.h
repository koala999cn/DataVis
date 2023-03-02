#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"


// 序列图的基类，主要处理串行数据，用于绘制graph、scatter、bars、area等类型图
// 兼容2d和3d模式，抽象grouped、stacked、ridged两类arrange模式

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	//////////////////////////////////////////////////////////////////////

	/// 维度映射（相对输出数据而言）

public:

	unsigned xdim() const { return axisDim_[0]; } // 返回x轴对应的数据维度
	unsigned ydim() const { return axisDim_[1]; } // 返回y轴对应的数据维度
	unsigned zdim() const { return axisDim_[2]; } // 返回z轴对应的数据维度

	void setXdim(unsigned dim); // 将维度dim映射到x轴
	void setYdim(unsigned dim); // 将维度dim映射到x轴
	void setZdim(unsigned dim); // 将维度dim映射到x轴

protected:

	point3 toPoint_(const float_t* valp, unsigned ch) const {
		return { valp[xdim()], valp[ydim()], usingDefaultZ_() ? defaultZ(ch) : valp[zdim()] };
	}

private:

	unsigned axisDim_[3]{ 0, 1, 2 }; // x/y/z轴对应的数据维度

	//////////////////////////////////////////////////////////////////////

protected:

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;

	virtual unsigned channels_() const;

	// 返回每个通道包含的1d数据数目. 对于samp2d数据，返回samp2d::size(0)
	virtual unsigned linesPerChannel_() const;

	// data1d数据总数
	unsigned linesTotal_() const;

	// 返回第ch通道的第idx条1d数据访问接口
	virtual KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const;

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


	////////////////////////////////////////////////////////////////////////
	// 
	// group绘制模式支持相关接口

protected:

	enum KeGroupMode
	{
		k_group_none,
		k_group_channel,
		k_group_column,
		k_group_all
	};

	void setGroupMode_(int mode);

private:
	int groupMode_{ k_group_none };

};
