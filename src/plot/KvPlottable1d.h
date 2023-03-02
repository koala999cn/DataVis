#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"


// ����ͼ�Ļ��࣬��Ҫ���������ݣ����ڻ���graph��scatter��bars��area������ͼ
// ����2d��3dģʽ������grouped��stacked��ridged����arrangeģʽ

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	//////////////////////////////////////////////////////////////////////

	/// ά��ӳ�䣨���������ݶ��ԣ�

public:

	unsigned xdim() const { return axisDim_[0]; } // ����x���Ӧ������ά��
	unsigned ydim() const { return axisDim_[1]; } // ����y���Ӧ������ά��
	unsigned zdim() const { return axisDim_[2]; } // ����z���Ӧ������ά��

	void setXdim(unsigned dim); // ��ά��dimӳ�䵽x��
	void setYdim(unsigned dim); // ��ά��dimӳ�䵽x��
	void setZdim(unsigned dim); // ��ά��dimӳ�䵽x��

protected:

	point3 toPoint_(const float_t* valp, unsigned ch) const {
		return { valp[xdim()], valp[ydim()], usingDefaultZ_() ? defaultZ(ch) : valp[zdim()] };
	}

private:

	unsigned axisDim_[3]{ 0, 1, 2 }; // x/y/z���Ӧ������ά��

	//////////////////////////////////////////////////////////////////////

protected:

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// ��GETTER����ת��ΪKvPaint��Ҫ�ĺ�����
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;

	virtual unsigned channels_() const;

	// ����ÿ��ͨ��������1d������Ŀ. ����samp2d���ݣ�����samp2d::size(0)
	virtual unsigned linesPerChannel_() const;

	// data1d��������
	unsigned linesTotal_() const;

	// ���ص�chͨ���ĵ�idx��1d���ݷ��ʽӿ�
	virtual KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const;

	aabb_t calcBoundingBox_() const override;


	////////////////////////////////////////////////////////////////////////
	// 
	// stack����ģʽ֧����ؽӿ�

	enum KeStackMode
	{
		k_stack_none, // �޶ѵ�
		k_stack_channel, // ��ͨ���ѵ�
		k_stack_column // ���У�x���򣩶ѵ������Զ�ά�������ݣ�
	};

	void setStackMode_(int mode);

private:

	void calcStackData_() const; // ����stack���ݣ��ڲ�����

	KuDataUtil::KpPointGetter1d lineStack_(unsigned ch, unsigned idx) const;

	int stackMode_{ k_stack_none };

	mutable std::vector<std::vector<float_t>> stackedData_; // ����stack���ݼ�����

	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////
	// 
	// ridge����ģʽ֧����ؽӿ�

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
	// group����ģʽ֧����ؽӿ�

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
