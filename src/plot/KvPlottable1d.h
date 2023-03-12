#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"
#include <map>


// ����ͼ�Ļ��࣬��Ҫ���������ݣ����ڻ���graph��scatter��bars��area������ͼ
// ����2d��3dģʽ��ʵ��ά��ӳ�䣬������grouped��stacked��ridged��arrangeģʽ

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	void setData(const_data_ptr d) override;

	void cloneConfig(const KvPlottable& plt) override;

protected:

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// ��GETTER����ת��ΪKvPaint��Ҫ�ĺ�����
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;


	// ����ÿ��ͨ��������1d������Ŀ. 
	// ����samp2d���ݣ�����size(0); samp3d���ݣ�����size(0)*size(1)
	unsigned linesPerChannel_() const;

	// data1d��������
	unsigned linesTotal_() const;

	// ÿ��line�����ݵ���Ŀ
	unsigned sizePerLine_() const;

	unsigned channels_() const;

	aabb_t calcBoundingBox_() const override;

	// ����stacked����
	bool output_() override;

	//////////////////////////////////////////////////////////////////////

	/// ά��ӳ�䣨���������ݶ��ԣ�

public:

	unsigned xdim() const { return axisDim_[0]; } // ����x���Ӧ������ά��
	unsigned ydim() const { return axisDim_[1]; } // ����y���Ӧ������ά��
	unsigned zdim() const { return axisDim_[2]; } // ����z���Ӧ������ά��

	virtual void setXdim(unsigned dim); // ��ά��dimӳ�䵽x��
	virtual void setYdim(unsigned dim); // ��ά��dimӳ�䵽x��
	virtual void setZdim(unsigned dim); // ��ά��dimӳ�䵽x��

protected:

	point3 toPoint_(const float_t* valp, unsigned ch) const {
		return { valp[xdim()], valp[ydim()], usingDefaultZ_() ? defaultZ(ch) : valp[zdim()] };
	}

private:

	unsigned axisDim_[3]{ 0, 1, 2 }; // x/y/z���Ӧ������ά��

	//////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////
	// 
	// arrangeģʽ

public:

	enum KeArrangeMode
	{
		k_arrange_none, 
		k_arrange_overlay = k_arrange_none,
		k_arrange_group, // x��ƫ��
		k_arrange_ridge, // y��ƫ��
		k_arrange_stack  // ֵ��ѵ�
	};

	// dim == odata()->dim()ʱ������channel��arrangeģʽ
	int arrangeMode(unsigned dim) const {
		return arrangeMode_[dim];
	}

	void setArrangeMode(unsigned dim, int mode);

	float_t ridgeOffset(unsigned dim) const { return ridgeOffset_[dim]; }
	void setRidgeOffset(unsigned dim, float_t offset);

	float_t groupOffset(unsigned dim) const { return groupOffset_[dim]; }
	void setGroupOffset(unsigned dim, float_t offset);

	float_t groupSpacing(unsigned dim) const { return groupSpacing_[dim]; }
	void setGroupSpacing(unsigned dim, float_t spacing);

	// �Ƿ���ڶѵ�ģʽ
	bool isStacked() const;

protected:
	
	// ������������һ����ֵΪch
	std::vector<kIndex> index_(unsigned ch, unsigned idx) const;

	// ���ص�chͨ���ĵ�idx��1d���ݷ��ʽӿ�
	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const {
		return lineArranged_(ch, idx, 0);
	}

	// ����stackedģʽ�£�λ��(ch, idx��֮�µ�����
	// assert(!isFloorStack_(ch, idx));
	KuDataUtil::KpPointGetter1d lineBelow_(unsigned ch, unsigned idx) const;


	// ���մӸ�ά����ά��˳��ͨ��Ϊ���ά�������δ���arrangeģʽ��dimά�ȣ���dim��
	KuDataUtil::KpPointGetter1d lineArranged_(unsigned ch, unsigned idx, unsigned dim) const;

	// ���ڿ����ж��ά��Ϊridgeģʽ�����Դ˴����ݲ���dim��������
	GETTER lineGrouped_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;
	GETTER lineRidged_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;
	GETTER lineStacked_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;

	float_t ridgeOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const;
	float_t ridgeOffsetAt_(unsigned ch, unsigned idx) const;

	float_t groupOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const;
	float_t groupOffsetAt_(unsigned ch, unsigned idx) const;
	
	bool isStacked_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_stack;
	}
	bool isRidged_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_ridge;
	}
	bool isGrouped_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_group;
	}
	bool isOverlayed_(unsigned dim) const {
		return arrangeMode_[dim] == k_arrange_overlay;
	}

	bool isFloorStack_(unsigned ch, unsigned idx) const;

private:

	void calcStackData_(unsigned dim) const; // ����stack���ݣ��ڲ�����

private:

	std::vector<int> arrangeMode_; // ��ά�ȵ�arrangeģʽ����С����odata()->dim()
	                               // ���һ��ֵ����channel��arrangeģʽ

	std::vector<float_t> ridgeOffset_;

	std::vector<float_t> groupOffset_, groupSpacing_;

	mutable std::map<unsigned, std::shared_ptr<KvDiscreted>> stackedData_; // ����stack���ݼ�����, dim -> data
};
