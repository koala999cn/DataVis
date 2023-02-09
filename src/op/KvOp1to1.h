#pragma once
#include "KvDataOperator.h"


// ���������Ϊ1�˿ڵĲ�������
// ʵ��ʱ����Ĵ���

class KvOp1to1 : public KvDataOperator
{
public:
	using KvDataOperator::KvDataOperator;

	void output() override {
		if (odataStamps_[0] < idataStamps_[0] || isOutputExpired(0)) { 
			outputImpl_();
			odataStamps_[0] = idataStamps_[0]; // clear output expired flag
			notifyChanged(0);
		}
	}

private:
	virtual void outputImpl_() = 0;
};