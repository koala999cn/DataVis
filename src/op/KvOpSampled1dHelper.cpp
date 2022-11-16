#include "KvOpSampled1dHelper.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


void KvOpSampled1dHelper::output()
{
	assert(idata_.size() == 1 && idata_.front());
	assert(idata_.front()->isDiscreted());

	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
	assert(in);
	if (in->size(in->dim() - 1) < isize_()) // 数据长度不足，暂不处理
		return; // TODO: 是否补零？

	in->dim() == 1 ? output1d_() : output2d_();
}


void KvOpSampled1dHelper::prepareOutput_()
{
	for (unsigned i = 0; i < outPorts(); i++) {
		std::shared_ptr<KvSampled> samp;

		if (dim(i) == 1) 
			samp = std::make_shared<KcSampled1d>();
		else 
			samp = std::make_shared<KcSampled2d>();

		for (kIndex j = 0; j < dim(i); j++)
			samp->reset(i, range(i, j).low(), step(i, j), 0.5);

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
	assert(out && out->size(0) == osize_(isize_()));

	unsigned offset = in->size(0) - isize_(); // 跳过多出的数据

	auto samp = std::dynamic_pointer_cast<KcSampled1d>(in);
	if (samp && samp->channels() == 1) {
		op_(samp->data() + offset, isize_(), out->data());
	}
	else {
		std::vector<kReal> in_(isize_());
		std::vector<kReal> out_(osize_(isize_()));
		for (kIndex ch = 0; ch < in->channels(); ch++) {
			for (kIndex i = 0; i < in->size(); i++)
				in_[i] = in->value(i + offset, ch);

			op_(in_.data(), isize_(), out_.data());
			out->setChannel(nullptr, ch, out_.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
		}
	}
}


void KvOpSampled1dHelper::output2d_()
{
	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	assert(in && in->size(1) >= isize_());
	assert(out && out->size(1) == osize_(isize_()));

	out->resize(in->size(0), osize_(isize_()), in->channels());
	unsigned offset = in->size(1) - isize_(); // 跳过多出的数据

	auto samp = std::dynamic_pointer_cast<KcSampled2d>(in);
	if (samp && samp->channels() == 1) {
		for (unsigned r = 0; r < in->size(0); r++)
			op_(samp->row(r) + offset, isize_(), out->row(r));
	}
	else {
		std::vector<kReal> iData(isize_()), oData(osize_(isize_()));

		for (kIndex ch = 0; ch < in->channels(); ch++) {
			kIndex row, col;
			for (row = 0; row < in->size(0); row++) {
				for (col = 0; col < iData.size(); col++)
					iData[col] = in->value(row, col + offset, ch);

				op_(iData.data(), isize_(), oData.data());
				out->setChannel(&row, ch, oData.data()); // TODO: 可以优化为直接写入out，不用经过中转缓存
			}
		}
	}
}
