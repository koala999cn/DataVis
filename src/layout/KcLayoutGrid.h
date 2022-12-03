#pragma once
#include "KcLayout2d.h"


// ���񲼾֣�ÿ�е�������ͬ��ͬ��Ԫ�صĲ��ֿ����ͬ

class KcLayoutGrid : public KcLayout2d
{
	using float_t = typename size_t::value_type;
	using super_ = KcLayout2d;

public:

	using super_::super_;

	unsigned cols() const;

	void resize(unsigned numRows, unsigned numCols);

	void removeColAt(unsigned colIdx);

	void insertRowAt(unsigned rowIdx);

	void insertColAt(unsigned colIdx);

	/// Ϊ���ִ洢�����������е�������ͬ��������ʵ��element�ķ��ʷ���

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

	void remove(KvLayoutElement* ele);


	void arrange(const rect_t& rc) final;

protected:
	size_t calcSize_(void* cxt) const override;

private:

	// ģ��ʵ��0ά�ȵ�arrangeStack_
	void arrangeColStack_(const rect_t& rc);

private:
	// calcSize�Ļ������. ���ڱ���ÿ�еĳߴ����ݣ�firstֵΪ�еĹ̶��ߴ磬secondֵΪ�е�extra�ݶ�
	mutable std::vector<std::pair<float_t, int>> szCols_;
};
