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

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// Ϊ�˼����������ݣ��˴�����unsigned��������ʾ����count
	// GETTER����z�滻������dim>1ʱ���������z���ӣ�����dim=1ʱ����ȷ�����ص����ݳߴ� >= 3
	// �̳��������forceDefaultZ��GETTER����ֵ�����δ����������zֵ�滻��������������
	virtual void drawImpl_(KvPaint*, GETTER, unsigned count, unsigned channel) const = 0;

	// ��GETTER����ת��ΪKvPaint��Ҫ�ĺ�����
	typename KvPaint::point_getter1 toPointGetter_(GETTER g, unsigned channel) const;

private:

	// һά����(x, y)��x-yƽ���ͼ��ʹ��ȱʡzֵ��
	void draw1d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)���ٲ�ͼ, ��xΪ������
	void draw2d_(KvPaint*, KvDiscreted*) const;

	// ��ά����(x, y, z)��3dͼ
	void draw3d_(KvPaint*, KvDiscreted*) const;

};
