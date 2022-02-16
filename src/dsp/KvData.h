#pragma once


// 所有数据源的基类. 仅提供一个接口，即获取数据维度的方法
class KvData
{
public:
	virtual unsigned dim() = 0;
};
