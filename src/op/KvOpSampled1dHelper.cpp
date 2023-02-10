#include "KvOpSampled1dHelper.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


KvOpSampled1dHelper::KvOpSampled1dHelper(const std::string_view& name, 
	bool splitChannels, bool permitSamp2d)
	: super_(name) 
	, splitChannels_(splitChannels)
	, permitSamp2d_(permitSamp2d)
{

}


bool KvOpSampled1dHelper::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec ds(dataSpec);
	return (ds.type == k_sampled || ds.type == k_array) && ds.dim <= (permitSamp2d_ ? 2 : 1);
}


kIndex KvOpSampled1dHelper::size(kIndex outPort, kIndex axis) const
{
	if (odata_[outPort]) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(odata_[outPort]);
		assert(disc);
		return disc->size(axis);
	}

	if (axis == dim(outPort) - 1 && isize_() != 0)
		return osize_(isize_());

	auto s = super_::size(outPort, axis);
	return (axis == dim(outPort) - 1) ? osize_(s) : s;
}


void KvOpSampled1dHelper::outputImpl_()
{
	assert(idata_.size() == 1 && idata_.front());
	assert(idata_.front()->isDiscreted());

	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
	assert(in);
	if (isize_() != 0 && in->size(in->dim() - 1) < isize_()) // 数据长度不足，暂不处理
		return; // TODO: 是否补零？

	in->dim() == 1 ? output1d_() : output2d_();
}


void KvOpSampled1dHelper::createOutputData_()
{
	for (unsigned i = 0; i < outPorts(); i++) {
		std::shared_ptr<KvSampled> samp;

		if (dim(i) == 1) 
			samp = std::make_shared<KcSampled1d>();
		else 
			samp = std::make_shared<KcSampled2d>();

		odata_[i] = nullptr;
		for (kIndex j = 0; j < dim(i); j++)
			samp->reset(j, range(i, j).low(), step(i, j), 0);

		std::vector<kIndex> idx(dim(i), 0);
		idx.back() = size(i, dim(i) - 1);
		samp->resize(idx.data(), channels(i));

		odata_[i] = samp;
	}
}


void KvOpSampled1dHelper::output1d_()
{
	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front()); // TODO: in可能为null
	auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
	assert(in && in->size(0) >= isize_());
	assert(out);

	auto isize = isize_() == 0 ? in->size(0) : isize_();
	unsigned offset = in->size(0) - isize; // 跳过多出的数据
	if (out->size(0) != osize_(isize))
		out->resize(osize_(isize));

	auto samp = std::dynamic_pointer_cast<KcSampled1d>(in);
	if (samp && (samp->channels() == 1 || !splitChannels_)) {
		op_(samp->data() + offset * samp->channels(), isize, out->data());
	}
	else if (splitChannels_) {
		std::vector<kReal> in_(isize);
		std::vector<kReal> out_(out->size(0));
		for (kIndex ch = 0; ch < in->channels(); ch++) {
			for (kIndex i = 0; i < isize; i++)
				in_[i] = in->value(i + offset, ch);

			op_(in_.data(), isize, out_.data());
			out->setChannel(nullptr, ch, out_.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
		}
	}
	else {
		std::vector<kReal> in_(isize * in->channels());
		auto buf = in_.data();
		for (kIndex i = 0; i < isize; i++) 
			for (kIndex ch = 0; ch < in->channels(); ch++)
				*buf++ = in->value(i + offset, ch); // TODO: 此处写入与多通道数据的布局有关
		
		op_(in_.data(), isize, out->data());
	}
}


void KvOpSampled1dHelper::output2d_()
{
	assert(isize_() != 0); // 流数据处理只支持固定输入尺寸

	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	assert(in && in->size(1) >= isize_()); 
	assert(out && out->size(1) == osize_(isize_()));

	auto isize = isize_();
	out->resize(in->size(0), osize_(isize), in->channels());
	unsigned offset = in->size(1) - isize; // 跳过多出的数据

	auto samp = std::dynamic_pointer_cast<KcSampled2d>(in);
	if (samp && (samp->channels() == 1 || !splitChannels_)) {
		for (unsigned r = 0; r < in->size(0); r++)
			op_(samp->row(r) + offset * samp->channels(), isize, out->row(r));
	}
	else if (splitChannels_) {
		std::vector<kReal> iData(isize), oData(osize_(isize));

		for (kIndex ch = 0; ch < in->channels(); ch++) {
			kIndex row, col;
			for (row = 0; row < in->size(0); row++) {
				for (col = 0; col < isize; col++)
					iData[col] = in->value(row, col + offset, ch);

				op_(iData.data(), isize, oData.data());
				out->setChannel(&row, ch, oData.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
			}
		}
	}
	else {
		std::vector<kReal> iData(isize * in->channels());

		kIndex row, col;
		for (row = 0; row < in->size(0); row++) {
			auto buf = iData.data();

			for (col = 0; col < isize; col++)
				for (kIndex ch = 0; ch < in->channels(); ch++)
				    *buf++ = in->value(row, col + offset, ch);

			op_(iData.data(), isize, out->row(row));
		}
	}
}


kIndex KvOpSampled1dHelper::isize_() const
{
	auto d = dim(0);
	return d == 0 ? 0 : inputSize_(d - 1);
}
