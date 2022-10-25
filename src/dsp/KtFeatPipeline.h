#pragma once
#include "KgPreprocess.h"
#include "KtPipeline.h"
#include <memory>
#include <functional>


template<typename... OP>
class KtFeatPipeline
{
public:

	KtFeatPipeline(const KgPreprocess::KpOptions& opts, OP&&... ops) 
	    : pipeline_(std::forward<OP>(ops)...) {
		prep_ = std::make_unique<KgPreprocess>(opts);
		assert(pipeline_.idim() == prep_->odim());

		// 设置缺省的特征生成handler
		prep_->setHandler([this](double* frame, double energy) {
			std::vector<double> out(odim());
			pipeline_.process(frame, out.data());
			if (prep_->options().useEnergy != KgPreprocess::k_use_energy_none)
				out[0] = energy; 
			handler_(out.data());
			});
	}

	void setHandler(std::function<void(double* out)> h) {
		handler_ = h;
	}

	void process(const double* buf, unsigned len) const {
		prep_->process(buf, len);
	}

	void flush() const {
		prep_->flush();
	}

	// 只有输出维度，输入维度由用户提供
	unsigned odim() const {
		return pipeline_.odim();
	}

protected:
	std::unique_ptr<KgPreprocess> prep_;
	KtPipeline<OP...> pipeline_;
	std::function<void(double*)> handler_;
};
