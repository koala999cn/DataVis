#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>

class KvData;

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
		return aabb_type{};
	}

private:
	std::string name_;
	data_ptr data_;
};
