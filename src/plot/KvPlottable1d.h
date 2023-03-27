#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter
#include "KuDataUtil.h"
#include <map>


// 序列图的基类，主要处理串行数据，用于绘制graph、scatter、bars、area等类型图
// 兼容2d和3d模式，实现维度映射，并抽象grouped、stacked、ridged等arrange模式

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	void setData(const_data_ptr d) override;

	void cloneConfig(const KvPlottable& plt) override;

protected:

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter toPoint3Getter_(GETTER g, unsigned channel) const;


	// 返回每个通道包含的1d数据数目. 
	// 对于samp2d数据，返回size(0); samp3d数据，返回size(0)*size(1)
	unsigned linesPerChannel_() const;

	// data1d数据总数
	unsigned linesTotal_() const;

	// 每条line的数据点数目
	unsigned sizePerLine_() const;

	unsigned channels_() const;

	aabb_t calcBoundingBox_() const override;

	// 更新stacked数据
	bool output_() override;

	//////////////////////////////////////////////////////////////////////

	/// 维度映射（相对输出数据而言）

public:

	unsigned xdim() const { return axisDim_[0]; } // 返回x轴对应的数据维度
	unsigned ydim() const { return axisDim_[1]; } // 返回y轴对应的数据维度
	unsigned zdim() const { return axisDim_[2]; } // 返回z轴对应的数据维度

	// 返回维度d被映射到的坐标轴: 0表示x轴，1表示y轴，2表示z轴，-1表示未被映射
	unsigned dimAxis(unsigned d) const;

	// 根据arrange模式返回偏移坐标轴，group模式返回0，ridge模式返回1，facet模式返回2，其他返回-1
	// @mapDim: 若true，则返回xdim, ydim, zdim，而非0, 1, 2
	unsigned deltaAxis(unsigned dim, bool mapDim) const;

	virtual void setXdim(unsigned dim); // 将维度dim映射到x轴
	virtual void setYdim(unsigned dim); // 将维度dim映射到x轴
	virtual void setZdim(unsigned dim); // 将维度dim映射到x轴

protected:

	point3 toPoint_(const float_t* valp, unsigned ch) const {
		return { valp[xdim()], valp[ydim()], usingDefaultZ_() ? defaultZ(ch) : valp[zdim()] };
	}

private:

	unsigned axisDim_[3]{ 0, 1, 2 }; // x/y/z轴对应的数据维度

	//////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////
	// 
	// arrange模式

public:

	enum KeArrangeMode
	{
		k_arrange_none, 
		k_arrange_overlay = k_arrange_none,
		k_arrange_group, // 分组：x轴偏移
		k_arrange_ridge, // 分层：y轴偏移
		k_arrange_facet, // 分面：z轴偏移
		k_arrange_stack  // 堆叠：值域累计
	};

	// dim == odata()->dim()时，返回channel的arrange模式
	int arrangeMode(unsigned dim) const {
		return arrangeMode_[dim];
	}

	void setArrangeMode(unsigned dim, int mode);

	float_t offset(unsigned dim) const { return offset_[dim]; }
	void setOffset(unsigned dim, float_t offset);

	float_t shift(unsigned dim) const { return shift_[dim]; }
	void setShift(unsigned dim, float_t sh);

	// 是否存在堆叠模式
	bool isStacked() const;

protected:
	
	// 返回数组的最后一个数值为ch
	std::vector<kIndex> index_(unsigned ch, unsigned idx) const;

	// 返回第ch通道的第idx条1d数据访问接口
	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const {
		return lineArranged_(ch, idx, 0);
	}

	// 返回stacked模式下，位于(ch, idx）之下的线条
	// assert(!isFloorStack_(ch, idx));
	KuDataUtil::KpPointGetter1d lineBelow_(unsigned ch, unsigned idx) const;


	// 按照从高维到低维的顺序（通道为最高维），依次处理arrange模式到dim维度（含dim）
	KuDataUtil::KpPointGetter1d lineArranged_(unsigned ch, unsigned idx, unsigned dim) const;

	GETTER lineStacked_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;

	// delta = offset + i * shift
	float_t deltaAt_(unsigned ch, unsigned idx, unsigned dim) const;

	point3 deltaAt_(unsigned ch, unsigned idx) const;


	bool isStacked_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_stack;
	}
	bool isRidged_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_ridge;
	}
	bool isGrouped_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_group;
	}
	bool isFaceted_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_facet;
	}
	bool isOverlayed_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_overlay;
	}

	bool isFloorStack_(unsigned ch, unsigned idx) const;

private:

	void calcStackData_(unsigned dim) const; // 计算stack数据，内部调用

	GETTER lineDeltaed_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;

private:

	std::vector<int> arrangeMode_; // 各维度的arrange模式，大小等于odata()->dim()
	                               // 最后一个值代表channel的arrange模式

	std::vector<float_t> offset_, shift_;

	mutable std::map<unsigned, std::shared_ptr<KvDiscreted>> stackedData_; // 保存stack数据计算结果, dim -> data
};
