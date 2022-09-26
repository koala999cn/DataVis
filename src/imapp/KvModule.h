#pragma once


// 程序模块的抽象接口，提供模块依赖关系
class KvModule
{
public:

	virtual bool init() = 0;
	virtual bool shutdown() = 0;

	void require();
};