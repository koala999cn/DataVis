#pragma once
#include "KvDataOperator.h"


// 输入输出均为1端口的操作算子
// 实现时间戳的处理

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