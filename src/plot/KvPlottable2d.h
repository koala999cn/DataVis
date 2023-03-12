#pragma once
#include "KvPlottable1d.h"
#include "KvPaint.h"

class KvSampled;

// imageͼ�Ļ��࣬��Ҫ����grid��sampled2d�����ݣ����ڻ�����ͼheatmap������ͼsurface��
// ����2d��3dģʽ
// Ϊʹ��arrangeģʽ������KvPlottable1d����2d�ӿ�
class KvPlottable2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	void setData(const_data_ptr d) override;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	bool showFill() const { return filled_; }
	bool& showFill() { return filled_; }

	bool showBorder() const { return edged_; }
	bool& showBorder() { return edged_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;


	/// ����lineʵ�ֵ�grid�ӿ�

	unsigned gridsPerChannel_() const;

	unsigned gridsTotal_() const;

	unsigned linesPerGrid_() const;

	// ���ص�chͨ���ĵ�gridIdx��grid�ĵ�lineIdx��1d���ݷ��ʽӿ�
	KuDataUtil::KpPointGetter1d gridLineAt_(unsigned ch, unsigned gridIdx, unsigned lineIdx) const;

	///////////////////////////////////////////////////////////////////////////////////////////////

private:
	bool filled_{ true };
	bool edged_{ false };
	KpPen borderPen_;
};
