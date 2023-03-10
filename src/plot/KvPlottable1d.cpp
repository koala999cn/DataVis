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


std::vector<kIndex> KvPlottable1d::index_(unsigned ch, unsigned idx) const
{
	auto disc = discreted_();
	auto shape = KuDataUtil::shape(*disc);
	shape.pop_back();
	auto index = KuDataUtil::n2index(shape, idx);
	index.push_back(ch);
	return index;
}


KuDataUtil::KpPointGetter1d KvPlottable1d::lineArranged_(unsigned ch, unsigned idx, unsigned dim) const
{
	assert(arrangeMode_.size() == odata()->dim());

	auto g = KuDataUtil::pointGetter1dAt(discreted_(), ch, idx);

	for (unsigned i = odata()->dim() - 1; i != -1 && i >= dim; i--) {
		if (isGrouped_(i))
			g.getter = lineGrouped_(g, ch, idx, i);
		else if (isRidged_(i))
			g.getter = lineRidged_(g, ch, idx, i);
		else if (isStacked_(i))
			g.getter = lineStacked_(g, ch, idx, i);
	}

	return g;
}


void KvPlottable1d::setArrangeMode(unsigned dim, int mode)
{
	arrangeMode_[dim] = mode;
	setDataChanged(false);
	setBoundingBoxExpired_();
	for (unsigned i = 0; i <= dim; i++)
	    stackedData_.erase(i); 
}


void KvPlottable1d::setRidgeOffset(unsigned dim, float_t offset)
{
	assert(isRidged_(dim));
	ridgeOffset_[dim] = offset;
	setDataChanged(false);
	setBoundingBoxExpired_();
	for (unsigned i = 0; i <= dim; i++)
		stackedData_.erase(i);
}


void KvPlottable1d::setGroupOffset(unsigned dim, float_t offset)
{
	assert(isGrouped_(dim));
	groupOffset_[dim] = offset;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KvPlottable1d::setGroupSpacing(unsigned dim, float_t spacing)
{
	assert(isGrouped_(dim));
	groupSpacing_[dim] = spacing;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


bool KvPlottable1d::output_()
{
	if (arrangeMode_.size() < idata()->dim()) { // TODO���˴�Ϊ����ĳЩ��outputImpl_�е���lineAt����Ҫ: 
		arrangeMode_.resize(idata()->dim());
		ridgeOffset_.resize(idata()->dim());
		groupOffset_.resize(idata()->dim());
		groupSpacing_.resize(idata()->dim());
	}

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

	if (dim == odata->dim() - 1) { // ��ͨ��stack

		for (unsigned ch = 0; ch < odata->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineArranged_(0, i, dim + 1);
				auto idx = KuDataUtil::n2index(shape, g.size * i);
				assert(idx.back() == 0);

				for (unsigned j = 0; j < g.size; j++) {
					auto val = ch == 0 ? g.getter(j).back() 
						: odata->value(idx.data(), ch) + stacked->value(idx.data(), ch - 1); // �ѵ�

					stacked->write(idx.data(), ch, val);
					idx.back()++;
				}
			}
		}
	}
	else { // ���ض�ά��stack

		// �����ضѵ������
		unsigned innerStackDim(dim + 1);
		for (; innerStackDim < odata->dim(); innerStackDim++)
			if (isStacked_(innerStackDim))
				break;


		for (unsigned ch = 0; ch < stacked->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineArranged_(ch, i, dim + 1);
				auto idx = KuDataUtil::n2index(shape, g.size * i);
				assert(idx.back() == 0);

				for (unsigned j = 0; j < g.size; j++) {
					float_t val;
					if (idx[dim] == 0) { // ��һ��ѵ�������£���ʼ��ʹ��ԭֵ
						val = g.getter(j).back();
					}
					else {
						val = odata->value(idx.data(), ch);
						
						if (innerStackDim >= odata->dim()) { // �޶��ضѵ�		
							idx[dim]--;
							val += stacked->value(idx.data(), ch); // �ѵ�		
							idx[dim]++; // �ָ�ԭֵ
						}
						// NB: ���ضѵ�����£�������ֵ���ۼӣ������Ӷѵ����Ӷѵ����ۼ�
						// ��ʱÿ���Ӷѵ���������һ���ѵ��Ļ������ۼӣ�����Ӷѵ�����ײ�Ҫ����һ������߲�Ϊ����
						// ������Ҫ�ر���ch == 0����idx[innerStackDim] == 0�����
						else if (innerStackDim == odata->dim() - 1) { // ��ͨ�����ضѵ�
							if (ch == 0) {
								idx[dim]--;
								val += stacked->value(idx.data(), stacked->channels() - 1);
								idx[dim]++; // �ָ�ԭֵ
							}
							else {
								val += stacked->value(idx.data(), ch - 1); // �Ӷѵ��ڲ��ۼ�
							}
						}
						else { // ά�ȶ��ضѵ�
							if (idx[innerStackDim] == 0) {
								idx[dim]--;
								idx[innerStackDim] = stacked->size(innerStackDim) - 1;
								val += stacked->value(idx.data(), ch);
								idx[innerStackDim] = 0; // �ָ�ԭֵ
								idx[dim]++; // �ָ�ԭֵ
							}
							else {
								idx[innerStackDim]--;
								val += stacked->value(idx.data(), ch); // �Ӷѵ��ڲ��ۼ�
								idx[innerStackDim]++; // �ָ�ԭֵ
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
	assert(isStacked_(dim));

	if (stackedData_.count(dim) == 0)
		calcStackData_(dim);

	assert(KuDataUtil::sameShape(*discreted_(), *stackedData_[dim], false));

	auto getter = g.getter;
	auto offset = idx * g.size;
	auto stacked = stackedData_[dim];
	return [getter, ch, offset, stacked](unsigned i) {
		auto pt = getter(i);
		pt.back() = stacked->valueAt(offset + i, ch); // �滻Ϊ�ѵ�ֵ
		return pt;
	};
}


KvPlottable1d::float_t KvPlottable1d::ridgeOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const
{
	assert(isRidged_(dim));

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
	assert(isGrouped_(dim));

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
	for (unsigned i = 0; i < odata()->dim(); i++) 
		if (isRidged_(i))
			offset += ridgeOffsetAt_(ch, idx, i);

	return offset;
}


KvPlottable1d::float_t KvPlottable1d::groupOffsetAt_(unsigned ch, unsigned idx) const
{
	float_t offset(0);
	for (unsigned i = 0; i < odata()->dim(); i++)
		if (isGrouped_(i))
			offset += groupOffsetAt_(ch, idx, i);

	return offset;
}


KvPlottable1d::GETTER KvPlottable1d::lineRidged_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const
{
	assert(isRidged_(dim));

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
	assert(isGrouped_(dim));

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
		// ���ù���lower.z��upper.z�Ĵ�С��aabb���캯�����Զ�������Сֵ
		box.lower().z() = defaultZ(0);
		box.upper().z() = defaultZ(odata()->channels() - 1); // TODO: �˴��̶�ʹ��ͨ����������zƽ����������ɿ������û�����
	}
	else {
		auto r2 = odata()->range(zdim());
		box.lower().z() = r2.low(), box.upper().z() = r2.high();
	}


	// ����groupedƫ��
	auto disc = discreted_();
	for (unsigned i = disc->dim() - 1; i != -1 ; i--) {
		if (isGrouped_(i)) {
			box.lower().x() += groupOffset_[i];
			box.upper().x() += groupOffset_[i];

			auto spacing = groupSpacing_[i] * ((i == disc->dim() - 1) ? disc->channels() - 1 : disc->size(i) - 1);
			if (spacing > 0)
				box.upper().x() += spacing;
			else
				box.lower().x() += spacing;
		}
	}


	// ����ridgedƫ��
	auto offset = ridgeOffsetAt_(0, 0);
	if (offset > 0)
		box.upper().y() += offset;
	else
		box.lower().y() += offset;
	

	// ����stackedƫ��
	if (ydim() == disc->dim() && isStacked()) {
		for (unsigned ch = 0; ch < disc->channels(); ch++)
			for (unsigned idx = 0; idx < linesPerChannel_(); idx++) {
				auto line = lineAt_(ch, idx);
				for (unsigned i = 0; i < line.size; i++)
					KuMath::updateRange(box.lower().y(), box.upper().y(), line.getter(i).back());
			}
	}

	return box;
}


bool KvPlottable1d::isFloorStack_(unsigned ch, unsigned idx) const
{
	std::vector<unsigned> stackDims;
	for (unsigned i = 0; i < odata()->dim(); i++)
		if (isStacked_(i))
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


KuDataUtil::KpPointGetter1d KvPlottable1d::lineBelow_(unsigned ch, unsigned idx) const
{
	assert(!isFloorStack_(ch, idx));

	auto disc = discreted_();
	std::vector<unsigned> stackDims;
	for (unsigned i = 0; i < disc->dim(); i++)
		if (isStacked_(i))
			stackDims.push_back(i);

	auto shape = KuDataUtil::shape(*disc);
	shape.back() = disc->channels(); // ��ch��idx����һ�����
	auto index = KuDataUtil::n2index(shape, idx * disc->channels() + ch);

	std::vector<kIndex> stackShape(stackDims.size());
	std::vector<kIndex> stackIndex(stackDims.size());
	for (unsigned i = 0; i < stackDims.size(); i++) {
		stackShape[i] = shape[stackDims[i]];
		stackIndex[i] = index[stackDims[i]];
	}

	KuDataUtil::prevIndex(stackShape, stackIndex.data());
	for (unsigned i = 0; i < stackDims.size(); i++) 
		index[stackDims[i]] = stackIndex[i];

	shape.pop_back(); // ����channelά��
	idx = KuDataUtil::index2n(shape, index.data());
	ch = index.back();

	return lineAt_(ch, idx);
}


bool KvPlottable1d::isStacked() const
{
	for (unsigned i = 0; i < odata()->dim(); i++)
		if (isStacked_(i))
			return true;

	return false;
}
