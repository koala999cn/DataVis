#pragma once
#include "KcLayout1d.h"


// ��ά���֣�ÿ�е��������Բ�ͬ�������㷨����KcLayout1dʵ��

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

	// ����Ԫ��ele��λ����ţ�-1��ʾδ�ҵ�ele
	// ���ҷ�Χ�޶��ڸ��е�Ԫ�أ����ݹ�������Ҳ��ƥ������������
	std::pair<unsigned, unsigned> find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

};
