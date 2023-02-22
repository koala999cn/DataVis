#include "KvOpSampled1dHelper.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KuDataUtil.h"


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
	return (ds.type == k_sampled || ds.type == k_array) && (ds.dim < 2 + permitSamp2d_);
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
	assert(idata_.size() == 1 && idata_.front() && idata_.front()->isDiscreted());

	// 同步odata的规格参数，尺寸信息由output1d_和output2d_同步
	for (unsigned i = 0; i < outPorts(); i++) {
		auto samp = std::dynamic_pointer_cast<KvSampled>(odata_[i]);
		for (kIndex j = 0; j < dim(i); j++)
			samp->reset(j, range(i, j).low(), step(i, j), 0);
	}

	idata_.front()->dim() == 1 ? output1d_() : output2d_();
}


bool KvOpSampled1dHelper::createOutputData_(kReal x0ref)
{
	for (unsigned i = 0; i < outPorts(); i++) {
		std::shared_ptr<KvSampled> samp;

		if (dim(i) == 1) 
			samp = std::make_shared<KcSampled1d>();
		else 
			samp = std::make_shared<KcSampled2d>();

		if (samp == nullptr)
			return false;

		odata_[i] = nullptr; // 防止range, step等成员直接使用odata_值
		for (kIndex j = 0; j < dim(i); j++)
			samp->reset(j, range(i, j).low(), step(i, j), x0ref);

		std::vector<kIndex> idx(dim(i), 0);
		idx.back() = size(i, dim(i) - 1);
		samp->resize(idx.data(), channels(i));

		odata_[i] = samp;
	}

	return true;
}


void KvOpSampled1dHelper::output1d_()
{
	auto in = idata_.front();
	assert(in);

	auto isize = isize_();
	if (isize == 0) isize = in->size();
	if (in->size() < isize) // 数据长度不足，暂不处理
		return; // TODO: 是否补零？

	auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
	if (!out) return;

	unsigned offset = in->size() - isize; // 数据的起始位置，丢弃多出的数据
	if (out->size() != osize_(isize))
		out->resize(osize_(isize));

	auto valueGetter = KuDataUtil::valueGetter1d(in);
	if (valueGetter.data) { // 先尝试快捷通道
		if (splitChannels_ && valueGetter.sampleStride == 1) { // 各通道采样点连续，可以直接使用
			auto inp = valueGetter.data + offset;

			if (out->stride(0) == 1) {
				auto outp = out->data();
				for (kIndex ch = 0; ch < valueGetter.channels; ch++) {
					op_(inp, isize, ch, outp);
					inp += valueGetter.channelStride;
					outp += out->stride(1);
				}
			}
			else {
				std::vector<kReal> obuf(out->size(0));
				for (kIndex ch = 0; ch < valueGetter.channels; ch++) {
					op_(inp, isize, ch, obuf.data());
					inp += valueGetter.channelStride;
					out->setChannel(nullptr, ch, obuf.data());
				}
			}

			return;
		}
		else if (!splitChannels_ && valueGetter.channelStride == 1) { // 各通道交错时可用
			op_(valueGetter.data + offset * valueGetter.channels, 
				isize, valueGetter.channels, out->data()); // TODO: 此处未检测out的布局
			return;
		}
	}

	// 使用getter
	if (!splitChannels_) {
		auto ibuf = valueGetter.fetch(offset, isize);
		op_(ibuf.data(), isize, valueGetter.channels, out->data()); // TODO: 此处未检测out的布局
	}	
	else {
		std::vector<kReal> obuf(out->size(0));
		for (kIndex ch = 0; ch < valueGetter.channels; ch++) {
			auto ibuf = valueGetter.fetchChannel(ch, offset, isize);
			assert(ibuf.size() == isize);
			op_(ibuf.data(), isize, ch, obuf.data());
			out->setChannel(nullptr, ch, obuf.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
		}
	}
}


void KvOpSampled1dHelper::output2d_()
{
	assert(isize_() != 0); // 流数据处理只支持固定输入尺寸

	auto valueGetter = KuDataUtil::valueGetter2d(idata_.front());
	assert(valueGetter.ysize == isize_());

	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	out->resize(valueGetter.xsize, osize_(valueGetter.ysize), valueGetter.channels);

	if (valueGetter.data) { // 先尝试快捷通道
		if (splitChannels_ && valueGetter.ystride == 1) { // 各通道采样点连续，可以直接使用
			std::vector<kReal> oData(osize_(valueGetter.ysize));

			for (kIndex ch = 0; ch < valueGetter.channels; ch++) {
				auto inp = valueGetter.data + ch * valueGetter.channelStride;
				for (kIndex r = 0; r < valueGetter.xsize; r++) {
					op_(inp, valueGetter.ysize, ch, oData.data());
					out->setChannel(&r, ch, oData.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
					inp += valueGetter.xstride;
				}
			}
			return;
		}
		else if (!splitChannels_ 
			&& valueGetter.channelStride == 1 
			&& valueGetter.ystride == valueGetter.channels) {
			auto inp = valueGetter.data;
			for (unsigned r = 0; r < valueGetter.xsize; r++) {
				op_(inp, valueGetter.ysize, valueGetter.channels, out->row(r));
				inp += valueGetter.xstride;
			}
		}
	}


	// 使用getter
	if (!splitChannels_) {
		for (kIndex r = 0; r < valueGetter.xsize; r++) {
			auto ibuf = valueGetter.fetchX(r);
			op_(ibuf.data(), valueGetter.ysize, valueGetter.channels, out->row(r));
		}
	}
	else {
		std::vector<kReal> oData(osize_(valueGetter.ysize));
		for (kIndex ch = 0; ch < valueGetter.channels; ch++) {
			for (kIndex r= 0; r < valueGetter.xsize; r++) {
				auto ibuf = valueGetter.fetchXOfChannel(ch, r);
				op_(ibuf.data(), valueGetter.ysize, ch, oData.data());
				out->setChannel(&r, ch, oData.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
			}
		}
	}
}


kIndex KvOpSampled1dHelper::isize_() const
{
	return inputSize_(dim(0) - 1);
}
