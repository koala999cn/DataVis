#include "KcBoxPlot.h"
#include "KvDiscreted.h"
#include "KvPaint.h"



const color4f& KcBoxPlot::minorColor() const
{
	return borderPen_.color;
}


void KcBoxPlot::setMinorColor(const color4f& minor)
{
	borderPen_.color = minor;
}


KcBoxPlot::aabb_t KcBoxPlot::boundingBox() const
{
	// ��ͼ��Ҫ��ͨ��˳��ֲ���x���0, 1, 2, ...λ�ã�����ʵ��x�᷶ΧΪ[0, channels-1]
	// Ϊ���������x�����Ҹ���չ0.5����λ�������޶�x��ֵ��Ϊ[-0.5, channels-0.5]

	auto aabb = super_::boundingBox();
	aabb.lower().x() = -0.5;
	aabb.upper().x() = data()->channels() - 0.5;

	for (auto& s : stats_) {
		if (aabb.lower().y() > s.lower)
			aabb.lower().y() = s.lower;
		if (aabb.upper().y() < s.upper)
			aabb.upper().y() = s.upper;
	}
	
	return aabb;
}


void KcBoxPlot::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	calcStats_(disc);

	for (unsigned i = 0; i < stats_.size(); i++) {
		auto z = defaultZ(i);
		auto& s = stats_[i];
		auto lower = point3{ i - boxWidth_/2, s.q1, z };
		auto upper = point3{ i + boxWidth_/2, s.q3, z };

		// fill box
		paint->setColor(majorColor(i));
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
		paint->drawLine(point3{ i, s.q1, z }, point3{ i, s.lower, z });
		paint->drawLine(point3{ i, s.q3, z }, point3{ i, s.upper, z });

		// draw the whisker bars
		paint->apply(whisBarPen_);
		paint->drawLine(point3{ i - whisBarWidth_/2, s.lower, z }, point3{ i + whisBarWidth_/2, s.lower, z });
		paint->drawLine(point3{ i - whisBarWidth_/2, s.upper, z }, point3{ i + whisBarWidth_/2, s.upper, z });

		// draw the outliers
		paint->apply(outlierMarker_);
		for (auto& y : s.outliers)
			paint->drawMarker(point3{ i, y, z }, outlierMarker_.showOutline);
	}
}


void KcBoxPlot::calcStats_(KvDiscreted* d) const
{
	stats_.resize(d->channels());
	for (unsigned ch = 0; ch < stats_.size(); ch++) {

		// �ռ���ǰͨ��������
		std::vector<float_t> vals(d->size());
		for (unsigned i = 0; i < d->size(); i++)
			vals[i] = d->valueAt(i, ch);

		// �������ͳ����
		auto& s = stats_[ch];
		std::sort(vals.begin(), vals.end()); // ��λ������Ҫ��˳������
		s.median = KuMath::quantile(vals.data(), vals.size(), 0.5);
		s.q1 = KuMath::quantile(vals.data(), vals.size(), 0.25);
		s.q3 = KuMath::quantile(vals.data(), vals.size(), 0.75);
		assert(s.q3 >= s.q1);

		if (whisFactor_ > 0) {
			s.lower = s.q1 - whisFactor_ * (s.q3 - s.q1);
			s.upper = s.q3 + whisFactor_ * (s.q3 - s.q1);

			// �����쳣ֵ
			for (auto i : vals)
				if (!KuMath::inRange<true>(i, s.lower, s.upper))
					s.outliers.push_back(i);
		}
		else { // ����������Ϊ�����Сֵ����ʱ���쳣ֵ
			s.lower = vals.front(), s.upper = vals.back();
		}
	}
}
