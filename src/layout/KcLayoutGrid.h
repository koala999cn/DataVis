#pragma once
#include "KcLayoutVector.h"


// ���񲼾֣��ײ��㷨����KcLayoutVectorʵ��

class KcLayoutGrid : public KcLayoutVector
{
	using super_ = KcLayoutVector;

public:

	KcLayoutGrid();

	void resize(unsigned numRows, unsigned numCols);

	unsigned rows() const { return super_::size(); }
	unsigned cols() const;

	KcLayoutVector* rowAt(unsigned rowIdx) const;

	void removeRowAt(unsigned rowIdx);

	void removeColAt(unsigned colIdx);

	/// element�ķ��ʷ���

	// ����[rowIdx, colIdx]λ�õ�Ԫ��ָ��
	// rowIdx < rows(), colIdx < cols()
	KvLayoutElement* getAt(unsigned rowIdx, unsigned colIdx) const;

	// �趨[rowIdx, colIdx]λ��Ԫ��Ϊele����λ�õ�ԭ��Ԫ�ؽ�������
	// ��[rowIdx, colIdx]������Χʱ���Զ�����Ԫ�������ռ�
	void putAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// ͬputAt�������Զ������ռ䣬����Χʱ������assertʧ��
	void setAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// ��[rowIdx, colIdx]λ�ò���eleԪ�أ�grid���С�����������1
	// rowIdx <= rows(), colIdx <= cols()
	void insertAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// ɾ��[rowIdx, colIdx]λ�õ�Ԫ�أ�Ԫ����������1��
	// rowIdx < rows(), colIdx < cols()
	void removeAt(unsigned rowIdx, unsigned colIdx);

	// ͬremoveAt����������Ԫ�ض��󣬶��Ƿ��ض���ָ��
	KvLayoutElement* takeAt(unsigned rowIdx, unsigned colIdx);

	// ����Ԫ��ele��λ����ţ�-1��ʾδ�ҵ�ele
	std::pair<unsigned, unsigned> find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

	// �൱��insertAt(rows(), cols(), ele)
	void append(KvLayoutElement* ele);

};
