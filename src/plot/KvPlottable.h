#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>
#include "KvData.h"


class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;

	KvPlottable() = default;
	KvPlottable(const std::string& name) : name_(name) {}

	const std::string& name() const { return name_; }
	void setName(const std::string& name) { name_ = name; }

	data_ptr data() const { return data_; }
	data_ptr& data() { return data_; }

	aabb_type boundingBox() const override {

		if (data_ == nullptr)
			return aabb_type(); // null

		point3 lower, upper;

		auto r0 = data_->range(0);
		auto r1 = data_->range(1);

		lower.x() = r0.low(), upper.x() = r0.high();
		lower.y() = r1.low(), upper.y() = r1.high();

		if (data_->dim() > 1) {
			auto r2 = data_->range(2);
			lower.z() = r2.low(), upper.z() = r2.high();
		}
		else {
			lower.z() = upper.z() = 0;
		}

		return { lower, upper };
	}

private:
	std::string name_;
	data_ptr data_;
};
