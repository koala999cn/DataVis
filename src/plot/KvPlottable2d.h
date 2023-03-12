#pragma once
#include "KvPlottable1d.h"
#include "KvPaint.h"

class KvSampled;

// image图的基类，主要处理grid（sampled2d）数据，用于绘制热图heatmap、曲面图surface等
// 兼容2d和3d模式
// 为使用arrange模式，基于KvPlottable1d构建2d接口
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


	/// 参照line实现的grid接口

	unsigned gridsPerChannel_() const;

	unsigned gridsTotal_() const;

	unsigned linesPerGrid_() const;

	// 返回第ch通道的第gridIdx个grid的第lineIdx条1d数据访问接口
	KuDataUtil::KpPointGetter1d gridLineAt_(unsigned ch, unsigned gridIdx, unsigned lineIdx) const;

	///////////////////////////////////////////////////////////////////////////////////////////////

private:
	bool filled_{ true };
	bool edged_{ false };
	KpPen borderPen_;
};
