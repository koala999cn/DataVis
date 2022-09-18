#pragma once
#include <vector>
#include <memory>
#include "KvPlottable.h"
#include "KtColor.h"


//
// һάͼ�ĳ�����
// �˴���һά��Ҫָ���ݰ��մ��з�ʽ��֯���������ݱ����ά��
// Ϊ����2ά���ݵ��3ά���ݵ㣬�ڲ�ͳһʹ��3ά���ݴ洢
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
	container_type data_; // ʹ��shared_ptr�Ա��ڲ�ͬ����ͼ�й�������
};
