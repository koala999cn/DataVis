#pragma once
#include <vector>
#include <memory>
#include "KvPlottable.h"
#include "KtColor.h"


//
// 一维图的抽象类
// 此处的一维主要指数据按照串行方式组织，而非数据本身的维度
// 为兼容2维数据点和3维数据点，内部统一使用3维数据存储
//

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:
	using container = std::vector<point3d>;
	using container_type = std::shared_ptr<container>;

	KvPlottable1d() {
		data_ = std::make_shared<container>();
		major_ = minor_ = { 0, 0, 0, 1 };
	}

	const container_type& data() const { return data_; }
	container_type& data() { return data_; }

	const color4f& majorColor() const { return major_; }
	const color4f& minorColor() const { return minor_; }

	bool empty() const {
		return !data_ || data_->empty();
	}

	void clear() {
		if (data_) data_->clear();
	}

private:
	color4f major_, minor_;
	container_type data_; // 使用shared_ptr以便在不同类型图中共享数据
};
