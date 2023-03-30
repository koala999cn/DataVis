#include "KcBoxPlot.h"
#include "KvDiscreted.h"
#include "KvPaint.h"


void KcBoxPlot::setYdim(unsigned dim)
{
	super_::setYdim(dim);
	setDataChanged(true); // y轴发生变化须重构输出
}


unsigned KcBoxPlot::objectCount() const
{
	return 1;
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
	auto aabb = super_::calcBoundingBox_();

	// NB: 为适应arrange模式，调整x轴尺度与输入一致
	// 
	// 箱图主要按通道顺序分布在x轴的0, 1, 2, ...位置，所以实际x轴范围为[0, channels-1]
	// 为了容纳箱框，x轴左右各拓展0.5个单位，按此修订x轴值域为[-0.5, channels-0.5]
	//aabb.lower().x() = -0.5;
	//aabb.upper().x() = odata()->channels() - 0.5;

	for (auto& s : stats_) 
		KuMath::uniteRange(aabb.lower().y(), aabb.upper().y(), s.lower * 0.99, s.upper * 1.01);
	
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
	auto half_dx = discreted_()->step(0) * 0.5;

	for (unsigned ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
			unsigned statIdx = ch * linesPerChannel_() + idx;
			auto delta = deltaAt_(ch, idx);
			float_t x = odata()->range(xdim()).mid() + delta[0];
			float_t z = usingDefaultZ_() ? defaultZ(ch) : odata()->range(zdim()).mid() + delta[2];

			auto& s = stats_[statIdx];
			auto lower = point3{ x - half_dx * boxWidth_, s.q1 + delta[1], z };
			auto upper = point3{ x + half_dx * boxWidth_, s.q3 + delta[1], z };

			// fill box
			paint->setColor(majorColor(objIdx));
			paint->fillRect(lower, upper);

			// draw the border of box
			paint->apply(borderPen_);
			paint->drawRect(lower, upper);

			// draw the median line
			lower.y() = upper.y() = s.median + delta[1];
			paint->apply(medianPen_);
			paint->drawLine(lower, upper);

			// draw the whisker lines
			paint->apply(whisPen_);
			paint->drawLine(point3{ x, s.q1 + delta[1], z }, point3{ x, s.lower + delta[1], z });
			paint->drawLine(point3{ x, s.q3 + delta[1], z }, point3{ x, s.upper + delta[1], z });

			// draw the whisker bars
			auto ww = half_dx * whisBarWidth_;
			paint->apply(whisBarPen_);
			paint->drawLine(point3{ x - ww, s.lower + delta[1], z }, point3{ x + ww, s.lower + delta[1], z });
			paint->drawLine(point3{ x - ww, s.upper + delta[1], z }, point3{ x + ww, s.upper + delta[1], z });

			// draw the outliers
			paint->apply(outlierMarker_);
			for (auto& y : s.outliers)
				paint->drawMarker(point3{ x, y + delta[1], z });
		}
	}

	return nullptr; // 无重用
}


void KcBoxPlot::outputImpl_()
{
	auto disc = discreted_();
	stats_.clear();
	stats_.reserve(linesTotal_());
	for (unsigned ch = 0; ch < odata()->channels(); ch++) 
		for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
			auto line = KuDataUtil::pointGetter1dAt(disc, ch, idx); // 使用原始数据进行统计

			// 收集当前通道的数据
			std::vector<float_t> vals(line.size);
			for (unsigned i = 0; i < line.size; i++)
				vals[i] = line.getter(i)[ydim()];

			// 计算各类统计量
			KpBoxStat_ s;
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

			stats_.push_back(std::move(s));
		}
}
