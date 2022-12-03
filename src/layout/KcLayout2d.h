#pragma once
#include "KcLayout1d.h"


// 二维布局，每行的列数可以不同。布局算法依赖KcLayout1d实现

class KcLayout2d : public KcLayout1d
{
	using super_ = KcLayout1d;

public:

	KcLayout2d();
	KcLayout2d(KvLayoutElement* parent);

	unsigned rows() const { return super_::size(); }

	void resize(unsigned numRows);

	super_* rowAt(unsigned rowIdx) const {
		return dynamic_cast<super_*>(super_::getAt(rowIdx));
	}

	void removeRowAt(unsigned rowIdx);

	void insertRowAt(unsigned rowIdx);

	// 返回元素ele的位置序号，-1表示未找到ele
	// 查找范围限定于各行的元素，不递归搜索，也不匹配行容器本身
	std::pair<unsigned, unsigned> find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

};
