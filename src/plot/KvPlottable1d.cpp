#include "KvPlottable1d.h"
#include "KvDiscreted.h"



void KvPlottable1d::setXdim(unsigned dim)
{
	axisDim_[0] = dim;
	setDataChanged();
}


void KvPlottable1d::setYdim(unsigned dim)
{
	axisDim_[1] = dim;
	setDataChanged();
}


void KvPlottable1d::setZdim(unsigned dim)
{
	axisDim_[2] = dim;
	setDataChanged();
}


typename KvPaint::point_getter1 KvPlottable1d::toPoint3Getter_(GETTER g, unsigned channel) const
{
	if (usingDefaultZ_()) {
		auto z = defaultZ(channel);
		return [this, g, z](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[xdim()], pt[ydim()], z);
		};
	}
	else {
		return [this, g](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[xdim()], pt[ydim()], pt[zdim()]);
		};
	}
}


unsigned KvPlottable1d::channels_() const
{
	return empty() ? 0 : data()->channels();
}


unsigned KvPlottable1d::linesPerChannel_() const
{
	return KuDataUtil::pointGetter1dCount(discreted_());
}


unsigned KvPlottable1d::linesTotal_() const
{
	return linesPerChannel_() * channels_();
}


void KvPlottable1d::setStackMode_(int mode)
{
	stackMode_ = mode;
	stackedData_.clear();
	setDataChanged();
}


void KvPlottable1d::setRidgeMode_(int mode)
{
	ridgeMode_ = mode;
	setDataChanged();
}


void KvPlottable1d::setGroupMode_(int mode) 
{
	groupMode_ = mode;
	setDataChanged();
}


void KvPlottable1d::setRidgeOffset(float_t offset)
{
	ridgeOffset_ = offset;
	setDataChanged();
}


void KvPlottable1d::calcStackData_() const
{
	auto disc = discreted_();
	stackedData_.resize(linesTotal_());

	if (stackMode_ == k_stack_channel) {

		for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
			auto g = KuDataUtil::pointGetter1dAt(disc, 0, idx);
			stackedData_[idx].resize(g.size);
			for (unsigned i = 0; i < g.size; i++)
				stackedData_[idx][i] = g.getter(i).back();
		}

		for (unsigned ch = 1; ch < disc->channels(); ch++) {
			for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
				auto g = KuDataUtil::pointGetter1dAt(disc, ch, idx);
				auto& d0 = stackedData_[(ch - 1) * linesPerChannel_() + idx];
				auto& d1 = stackedData_[ch * linesPerChannel_() + idx];
				d1.resize(g.size);
				assert(d1.size() == d0.size());
				for (unsigned i = 0; i < g.size; i++)
					d1[i] = g.getter(i).back() + d0[i]; // 堆叠
			}
		}
	}
	else if (stackMode_ == k_stack_column) {

		for (unsigned ch = 0; ch < disc->channels(); ch++) {
			auto g = KuDataUtil::pointGetter1dAt(disc, ch, 0);
			auto& d = stackedData_[ch * linesPerChannel_()];
			d.resize(g.size);
			for (unsigned i = 0; i < g.size; i++)
				d[i] = g.getter(i).back();
		}

		for (unsigned ch = 0; ch < disc->channels(); ch++) {
			for (unsigned idx = 1; idx < linesPerChannel_(); idx++) {
				auto g = KuDataUtil::pointGetter1dAt(disc, ch, idx);
				auto& d0 = stackedData_[ch * linesPerChannel_() + idx - 1];
				auto& d1 = stackedData_[ch * linesPerChannel_() + idx];
				d1.resize(g.size);
				assert(d1.size() == d0.size());
				for (unsigned i = 0; i < g.size; i++)
					d1[i] = g.getter(i).back() + d0[i]; // 堆叠
			}
		}
	}
}


KuDataUtil::KpPointGetter1d KvPlottable1d::lineStack_(unsigned ch, unsigned idx) const
{
	auto g = KuDataUtil::pointGetter1dAt(discreted_(), ch, idx);
	if (stackMode_ == k_stack_none)
		return g;

	if (stackedData_.empty())
		calcStackData_();

	KuDataUtil::KpPointGetter1d sg;
	unsigned pos = ch * linesPerChannel_() + idx;
	sg.getter = [g, pos, this](unsigned i) {
		auto pt = g.getter(i);
		pt.back() = stackedData_[pos][i]; // 替换为堆叠值
		return pt;
	};
	sg.size = g.size;

	return sg;
}


KvPlottable1d::float_t KvPlottable1d::ridgeOffsetAt_(unsigned ch, unsigned idx) const
{
	float_t offset(0);
	if (ridgeMode_ == k_ridge_channel)
		offset = ridgeOffset_ * (data()->channels() - ch - 1);
	else if (ridgeMode_ == k_ridge_column)
		offset = ridgeOffset_ * (linesPerChannel_() - idx - 1);
	else
		offset = ridgeOffset_ * ((data()->channels() - ch) * linesPerChannel_() - idx - 1);

	return offset;
}


KuDataUtil::KpPointGetter1d KvPlottable1d::lineAt_(unsigned ch, unsigned idx) const
{
	// 处理ridge模式
	auto g = lineStack_(ch, idx);
	
	if (ridgeMode_ == k_ridge_none)
		return g;
	
	auto offset = ridgeOffsetAt_(ch, idx);

	KuDataUtil::KpPointGetter1d rg;
	rg.getter = [this, g, offset](unsigned i) {
		auto pt = g.getter(i);
		pt[ydim()] += offset;
		return pt;
	};
	rg.size = g.size;

	return rg;
}


KvPlottable::aabb_t KvPlottable1d::calcBoundingBox_() const
{
	if (empty())
		return aabb_t(); // 返回null，以免影响坐标系设置 

	aabb_t box;

	// xrange
	auto r0 = data()->range(xdim());
	box.lower().x() = r0.low(), box.upper().x() = r0.high();

	// yrange
	if (stackMode_ == k_stack_none) {
		auto r1 = data()->range(ydim());
		box.lower().y() = r1.low(), box.upper().y() = r1.high();
	}
	else {
		if (stackedData_.empty())
			calcStackData_();
		auto r1 = KuMath::minmax(stackedData_[0].data(), stackedData_[0].size());
		for (unsigned i = 1; i < stackedData_.size(); i++) {
			auto r = KuMath::minmax(stackedData_[i].data(), stackedData_[i].size());
			KuMath::uniteRange(r1.first, r1.second, r.first, r.second);
		}
		box.lower().y() = r1.first, box.upper().y() = r1.second;
	}

	// zrange
	if (usingDefaultZ_()) {
		// 不用关心lower.z与upper.z的大小，aabb构造函数会自动调整大小值
		box.lower().z() = defaultZ(0);
		box.upper().z() = defaultZ(channels_() - 1); // TODO: 此处固定使用通道数来配置z平面的数量，可考虑由用户定制
	}
	else {
		auto r2 = data()->range(zdim());
		box.lower().z() = r2.low(), box.upper().z() = r2.high();
	}

	if (ridgeMode_ != k_ridge_none) {
		auto off = ridgeOffsetAt_(0, 0);
		if (off > 0) box.upper().y() += off;
		else box.lower().y() += off;
	}

	return box;
}
