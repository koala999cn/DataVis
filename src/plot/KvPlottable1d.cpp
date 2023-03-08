#include "KvPlottable1d.h"
#include "KvSampled.h"


void KvPlottable1d::setXdim(unsigned dim)
{
	axisDim_[0] = dim;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KvPlottable1d::setYdim(unsigned dim)
{
	axisDim_[1] = dim;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KvPlottable1d::setZdim(unsigned dim)
{
	axisDim_[2] = dim;
	setDataChanged(false);
	setBoundingBoxExpired_();
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


unsigned KvPlottable1d::linesPerChannel_() const
{
	return KuDataUtil::pointGetter1dCount(discreted_());
}


unsigned KvPlottable1d::linesTotal_() const
{
	return empty() ? 0 : linesPerChannel_() * odata()->channels();
}


KuDataUtil::KpPointGetter1d KvPlottable1d::lineArranged_(unsigned ch, unsigned idx, unsigned dim) const
{
	assert(arrangeMode_.size() == odata()->dim());

	auto g = KuDataUtil::pointGetter1dAt(discreted_(), ch, idx);

	for (unsigned i = arrangeMode_.size() - 1; i != -1 && i >= dim; i--) {
		if (arrangeMode_[i] == k_arrange_group)
			g.getter = lineGrouped_(g, ch, idx, i);
		else if (arrangeMode_[i] == k_arrange_ridge)
			g.getter = lineRidged_(g, ch, idx, i);
		else if (arrangeMode_[i] == k_arrange_stack)
			g.getter = lineStacked_(g, ch, idx, i);
	}

	return g;
}


void KvPlottable1d::setArrangeMode(unsigned dim, int mode)
{
	arrangeMode_[dim] = mode;
	setDataChanged(false);
	setBoundingBoxExpired_();
	stackedData_.clear(); // TODO: 更精细的控制
}


void KvPlottable1d::setRidgeOffset(unsigned dim, float_t offset)
{
	assert(arrangeMode_[dim] == k_arrange_ridge);
	ridgeOffset_[dim] = offset;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KvPlottable1d::setGroupOffset(unsigned dim, float_t offset)
{
	assert(arrangeMode_[dim] == k_arrange_group);
	groupOffset_[dim] = offset;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KvPlottable1d::setGroupSpacing(unsigned dim, float_t spacing)
{
	assert(arrangeMode_[dim] == k_arrange_group);
	groupSpacing_[dim] = spacing;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


bool KvPlottable1d::output_()
{
	if (!super_::output_())
		return false;

	if (!empty()) {
		arrangeMode_.resize(odata()->dim());
		ridgeOffset_.resize(odata()->dim());
		groupOffset_.resize(odata()->dim());
		groupSpacing_.resize(odata()->dim());
	}
	stackedData_.clear();
	return true;
}


void KvPlottable1d::calcStackData_(unsigned dim) const
{
	stackedData_[dim] = KuDataUtil::shapeLike(*discreted_());
	auto stacked = std::dynamic_pointer_cast<KvSampled>(stackedData_[dim]);
	assert(stacked && stacked->writable());
	auto odata = std::dynamic_pointer_cast<const KvSampled>(discreted_());
	assert(odata);

	auto shape = KuDataUtil::shape(*stacked);

	if (dim == odata->dim() - 1) { // 按通道stack

		for (unsigned ch = 0; ch < odata->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineArranged_(0, i, dim + 1);
				auto idx = KuDataUtil::n2index(shape, g.size * i);
				assert(idx.back() == 0);

				for (unsigned j = 0; j < g.size; j++) {
					auto val = ch == 0 ? g.getter(j).back() 
						: odata->value(idx.data(), ch) + stacked->value(idx.data(), ch - 1); // 堆叠

					stacked->write(idx.data(), ch, val);
					idx.back()++;
				}
			}
		}
	}
	else { // 按特定维度stack

		// 检测多重堆叠的情况
		unsigned innerStackDim(dim + 1);
		for (; innerStackDim < odata->dim(); innerStackDim++)
			if (arrangeMode_[innerStackDim] == k_arrange_stack)
				break;


		for (unsigned ch = 0; ch < stacked->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineArranged_(ch, i, dim + 1);
				auto idx = KuDataUtil::n2index(shape, g.size * i);
				assert(idx.back() == 0);

				for (unsigned j = 0; j < g.size; j++) {
					float_t val;
					if (idx[dim] == 0) { // 第一层堆叠（最底下），始终使用原值
						val = g.getter(j).back();
					}
					else {
						val = odata->value(idx.data(), ch);
						
						if (innerStackDim >= odata->dim()) { // 无多重堆叠		
							idx[dim]--;
							val += stacked->value(idx.data(), ch); // 堆叠		
							idx[dim]++; // 恢复原值
						}
						// NB: 多重堆叠情况下，不再是值的累加，而是子堆叠与子堆叠的累加
						// 此时每个子堆叠都是在上一个堆叠的基础上累加，因此子堆叠的最底层要以上一个的最高层为参照
						// 所以需要特别处理ch == 0或者idx[innerStackDim] == 0的情况
						else if (innerStackDim == odata->dim() - 1) { // 与通道多重堆叠
							if (ch == 0) {
								idx[dim]--;
								val += stacked->value(idx.data(), stacked->channels() - 1);
								idx[dim]++; // 恢复原值
							}
							else {
								val += stacked->value(idx.data(), ch - 1); // 子堆叠内部累加
							}
						}
						else { // 维度多重堆叠
							if (idx[innerStackDim] == 0) {
								idx[dim]--;
								idx[innerStackDim] = stacked->size(innerStackDim) - 1;
								val += stacked->value(idx.data(), ch);
								idx[innerStackDim] = 0; // 恢复原值
								idx[dim]++; // 恢复原值
							}
							else {
								idx[innerStackDim]--;
								val += stacked->value(idx.data(), ch); // 子堆叠内部累加
								idx[innerStackDim]++; // 恢复原值
							}
						}
					}

					stacked->write(idx.data(), ch, val);
					idx.back()++;
				}
			}
		}
	}

	setBoundingBoxExpired_();
}


KvPlottable1d::GETTER KvPlottable1d::lineStacked_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const
{
	assert(arrangeMode_[dim] == k_arrange_stack);

	if (stackedData_.count(dim) == 0)
		calcStackData_(dim);

	assert(KuDataUtil::sameShape(*discreted_(), *stackedData_[dim], false));

	auto getter = g.getter;
	auto offset = idx * g.size;
	auto stacked = stackedData_[dim];
	return [getter, ch, offset, stacked](unsigned i) {
		auto pt = getter(i);
		pt.back() = stacked->valueAt(offset + i, ch); // 替换为堆叠值
		return pt;
	};
}


KvPlottable1d::float_t KvPlottable1d::ridgeOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const
{
	assert(arrangeMode_[dim] == k_arrange_ridge);

	float_t offset(0);
	if (dim == odata()->dim() - 1) {
		offset = ridgeOffset_[dim] * (odata()->channels() - ch - 1);
	}
	else if (odata()->dim() == 1) {
		assert(dim == 0);
		offset = ridgeOffset_[dim] * (linesPerChannel_() - idx - 1);
	}
	else {
		auto disc = discreted_();
		auto shape = KuDataUtil::shape(*disc);
		shape.pop_back();
		auto index = KuDataUtil::n2index(shape, idx);
		offset = ridgeOffset_[dim] * (shape[dim] - index[dim] - 1);
	}

	return offset;
}


KvPlottable1d::float_t KvPlottable1d::groupOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const
{
	assert(arrangeMode_[dim] == k_arrange_group);

	float_t offset(0);
	if (dim == odata()->dim() - 1) {
		offset = groupOffset_[dim] + groupSpacing_[dim] * ch;
	}
	else if (odata()->dim() == 1) {
		assert(dim == 0);
		offset = groupOffset_[dim] + groupSpacing_[dim] * idx;
	}
	else {
		auto disc = discreted_();
		auto shape = KuDataUtil::shape(*disc);
		shape.pop_back();
		auto index = KuDataUtil::n2index(shape, idx);
		offset = groupOffset_[dim] + groupSpacing_[dim] *  index[dim];
	}

	return offset;
}


KvPlottable1d::float_t KvPlottable1d::ridgeOffsetAt_(unsigned ch, unsigned idx) const
{
	float_t offset(0);
	for (unsigned i = 0; i < arrangeMode_.size(); i++) 
		if (arrangeMode_[i] == k_arrange_ridge)
			offset += ridgeOffsetAt_(ch, idx, i);

	return offset;
}


KvPlottable1d::GETTER KvPlottable1d::lineRidged_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const
{
	auto offset = ridgeOffsetAt_(ch, idx, dim);
	auto getter = g.getter;

	return [this, getter, offset](unsigned i) {
		auto pt = getter(i);
		pt[ydim()] += offset;
		return pt;
	};
}


KvPlottable1d::GETTER KvPlottable1d::lineGrouped_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const
{
	auto offset = groupOffsetAt_(ch, idx, dim);
	auto getter = g.getter;

	return [this, getter, offset](unsigned i) {
		auto pt = getter(i);
		pt[xdim()] += offset;
		return pt;
	};

	return g.getter;
}


KvPlottable::aabb_t KvPlottable1d::calcBoundingBox_() const
{
	aabb_t box;

	// xrange
	auto r0 = odata()->range(xdim());
	box.lower().x() = r0.low(), box.upper().x() = r0.high();

	// yrange
	auto r1 = odata()->range(ydim());
	box.lower().y() = r1.low(), box.upper().y() = r1.high();

	// zrange
	if (usingDefaultZ_()) {
		// 不用关心lower.z与upper.z的大小，aabb构造函数会自动调整大小值
		box.lower().z() = defaultZ(0);
		box.upper().z() = defaultZ(odata()->channels() - 1); // TODO: 此处固定使用通道数来配置z平面的数量，可考虑由用户定制
	}
	else {
		auto r2 = odata()->range(zdim());
		box.lower().z() = r2.low(), box.upper().z() = r2.high();
	}

	auto disc = discreted_();
	for (unsigned i = arrangeMode_.size() - 1; i != -1 ; i--) {
		if (arrangeMode_[i] == k_arrange_group) {
			box.lower().x() += groupOffset_[i];
			box.upper().x() += groupOffset_[i];

			auto spacing = groupSpacing_[i] * ((i == arrangeMode_.size() - 1) ? disc->channels() - 1 : disc->size(i) - 1);
			if (spacing > 0)
				box.upper().x() += spacing;
			else
				box.lower().x() += spacing;
		}
		else if (arrangeMode_[i] == k_arrange_ridge) {
			auto offset = ridgeOffsetAt_(0, 0, i);
			if (offset > 0) 
				box.upper().y() += offset;
			else 
				box.lower().y() += offset;
		}
	}

	if (ydim() == disc->dim()) {
		for (unsigned i = 0; i < arrangeMode_.size(); i++) {
			if (arrangeMode_[i] == k_arrange_stack) {
				for (unsigned ch = 0; ch < disc->channels(); ch++)
					for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
						auto line = lineArranged_(ch, idx, i);
						for (unsigned i = 0; i < line.size; i++)
							KuMath::updateRange(box.lower().y(), box.upper().y(), line.getter(i).back());
					}

				break; // 只须计算最外围的数据
			}
		}
	}

	return box;
}


bool KvPlottable1d::isFloorStack_(unsigned ch, unsigned idx) const
{
	std::vector<unsigned> stackDims;
	for (unsigned i = 0; i < odata()->dim(); i++)
		if (arrangeMode(i) == k_arrange_stack)
			stackDims.push_back(i);

	for (auto d : stackDims) {
		if (d == odata()->dim() - 1	&& ch != 0) {
			return false;
		}
		else {
			auto sh = KuDataUtil::shape(*discreted_());
			sh.pop_back();
			if (KuDataUtil::n2index(sh, idx).at(d) != 0)
				return false;
		}
	}

	return true;
}
