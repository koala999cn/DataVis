#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// ����ͼ�Ļ��࣬��Ҫ���������ݣ����ڻ�������ͼgraph��ɢ��ͼscatter��
// ����2d��3dģʽ

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

protected:

	unsigned objectCount() const override;

	// ���Ƶ�objIdx����Ⱦ���󣬲����ؿɸ��õĶ���id
	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	// ÿ�����ε���Ⱦ������Ŀ���е�ʵ�ֿ���fill��edge�ֱ���1�������еĿ���text����1������
	// ͨ��ÿ��ͨ������1������
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

private:

	// һά����(x, y)��x-yƽ���ͼ��ʹ��ȱʡzֵ��
	void* draw1d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// ��ά����(x, y, z)���ٲ�ͼ, ��xΪ������
	void* draw2d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// ��ά����(x, y, z)��3dͼ
	void* draw3d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

};
