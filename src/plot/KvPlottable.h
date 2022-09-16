#pragma once
#include "KvRenderable.h"
#include <string>


class KvPlottable : public KvRenderable
{
public:
	KvPlottable() = default;
	KvPlottable(const std::string& name) : name_(name) {}

	const std::string& name() const { return name_; }
	void setName(const std::string& name) { name_ = name; }

private:
	std::string name_;
};
