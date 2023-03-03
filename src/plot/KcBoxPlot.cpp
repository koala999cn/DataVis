#include "KcBoxPlot.h"
#include "KvDiscreted.h"
#include "KvPaint.h"


unsigned KcBoxPlot::objectCount() const
{
	return odata()->channels();
}


const color4f& KcBoxPlot::minorColor() const
{
	return borderPen_.color;
}


void KcBoxPlot::setMinorColor_(const color4f& minor)
{
	borderPen_.color = minor;
}


KcBoxPlot::aabb_t KcBoxPlot::calcBoundingBox_() const
{
	// 箱图主要按通道顺序分布在x轴的0, 1, 2, ...位置，所以实际x轴范围为[0, channels-1]
	// 为了容纳箱框，x轴左右各拓展0.5个单位，按此修订x轴值域为[-0.5, channels-0.5]

	auto aabb = super_::calcBoundingBox_();
	aabb.lower().x() = -0.5;
	aabb.upper().x() = odata()->channels() - 0.5;

	for (auto& s : stats_) {
		if (aabb.lower().y() > s.lower)
			aabb.lower().y() = s.lower;
		if (aabb.upper().y() < s.upper)
			aabb.upper().y() = s.upper;
	}
	
	return aabb;
}


void KcBoxPlot::setObjectState_(KvPaint*, unsigned objIdx) const
{
	// do nothing
}


bool KcBoxPlot::objectVisible_(unsigned objIdx) const
{
	return true;
}

void* KcBoxPlot::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	if (dataChanged() && objIdx == 0) // 只计算一次
	    calcStats_();

	assert(objIdx < stats_.size());

	auto z = defaultZ(objIdx);
	auto& s = stats_[objIdx];
	auto lower = point3{ objIdx - boxWidth_/2, s.q1, z };
	auto upper = point3{ objIdx + boxWidth_/2, s.q3, z };

	// fill box
	paint->setColor(majorColor(objIdx));
	paint->fillRect(lower, upper);

	// draw the border of box
	paint->apply(borderPen_);
	paint->drawRect(lower, upper);

	// draw the median line
	lower.y() = upper.y() = s.median;
	paint->apply(medianPen_);
	paint->drawLine(lower, upper);

	// draw the whisker lines
	paint->apply(whisPen_);
	paint->drawLine(point3{ objIdx, s.q1, z }, point3{ objIdx, s.lower, z });
	paint->drawLine(point3{ objIdx, s.q3, z }, point3{ objIdx, s.upper, z });

	// draw the whisker bars
	paint->apply(whisBarPen_);
	paint->drawLine(point3{ objIdx - whisBarWidth_/2, s.lower, z }, point3{ objIdx + whisBarWidth_/2, s.lower, z });
	paint->drawLine(point3{ objIdx - whisBarWidth_/2, s.upper, z }, point3{ objIdx + whisBarWidth_/2, s.upper, z });

	// draw the outliers
	paint->apply(outlierMarker_);
	for (auto& y : s.outliers)
		paint->drawMarker(point3{ objIdx, y, z });

	return nullptr; // 无重用
}


void KcBoxPlot::calcStats_() const
{
	auto d = discreted_();
	stats_.resize(d->channels());
	for (unsigned ch = 0; ch < stats_.size(); ch++) {

		// 收集当前通道的数据
		std::vector<float_t> vals(d->size());
		for (unsigned i = 0; i < d->size(); i++)
			vals[i] = d->valueAt(i, ch);

		// 计算各类统计量
		auto& s = stats_[ch];
		std::sort(vals.begin(), vals.end()); // 分位数计算要求顺序数据
		s.median = KuMath::quantile(vals.data(), vals.size(), 0.5);
		s.q1 = KuMath::quantile(vals.data(), vals.size(), 0.25);
		s.q3 = KuMath::quantile(vals.data(), vals.size(), 0.75);
		assert(s.q3 >= s.q1);

		if (whisFactor_ > 0) {
			s.lower = s.q1 - whisFactor_ * (s.q3 - s.q1);
			s.upper = s.q3 + whisFactor_ * (s.q3 - s.q1);

			// 挑出异常值
			for (auto i : vals)
				if (!KuMath::inRange<true>(i, s.lower, s.upper))
					s.outliers.push_back(i);
		}
		else { // 设置上下须为最大最小值，此时无异常值
			s.lower = vals.front(), s.upper = vals.back();
		}
	}
}
