#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"


// ����ͼ�Ļ��࣬��Ҫ���������ݣ����ڻ�������ͼgraph��ɢ��ͼscatter��
// ����2d��3dģʽ������stacked��ͼģʽ

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

protected:

	unsigned objectCount() const override;

	// ���Ƶ�objIdx����Ⱦ���󣬲����ؿɸ��õĶ���id
	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	// ÿ�����ε���Ⱦ������Ŀ���е�ʵ�ֿ���fill��edge�ֱ���1�������еĿ���text����1������
	// ͨ��ÿ��ͨ����Ӧ1������
	virtual unsigned objectsPerBatch_() const { return 1; }

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// Ϊ�˼����������ݣ��˴�����unsigned��������ʾ����count
	// GETTER����z�滻������dim>1ʱ���������z���ӣ�����dim=1ʱ����ȷ�����ص����ݳߴ� >= 3
	// �̳��������forceDefaultZ��GETTER����ֵ�����δ����������zֵ�滻��������������
	virtual void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const = 0;

	// ��GETTER����ת��ΪKvPaint��Ҫ�ĺ�����
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;

	// ����Ⱦ����idת��Ϊͨ����
	unsigned objIdx2ChsIdx_(unsigned objIdx) const;

	// ����ÿ��ͨ��������1d������Ŀ. ����samp2d���ݣ�����samp2d::size(0)
	unsigned linesPerChannel_() const {
		return KuDataUtil::pointGetter1dCount(discreted_());
	}

	// data1d��������
	unsigned linesTotal_() const;

	// ���ص�chͨ���ĵ�idx��1d���ݷ��ʽӿ�
	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const;

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

private:

	// һά����(x, y)��x-yƽ���ͼ��ʹ��ȱʡzֵ��
	void* draw1d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// ��ά����(x, y, z)���ٲ�ͼ, ��xΪ������
	void* draw2d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// ��ά����(x, y, z)��3dͼ
	void* draw3d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

};
