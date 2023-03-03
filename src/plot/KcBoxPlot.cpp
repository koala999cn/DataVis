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
	// ��ͼ��Ҫ��ͨ��˳��ֲ���x���0, 1, 2, ...λ�ã�����ʵ��x�᷶ΧΪ[0, channels-1]
	// Ϊ���������x�����Ҹ���չ0.5����λ�������޶�x��ֵ��Ϊ[-0.5, channels-0.5]

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
	if (dataChanged() && objIdx == 0) // ֻ����һ��
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

	return nullptr; // ������
}


void KcBoxPlot::calcStats_() const
{
	auto d = discreted_();
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
