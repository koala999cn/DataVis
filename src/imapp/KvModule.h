#pragma once


// ����ģ��ĳ���ӿڣ��ṩģ��������ϵ
class KvModule
{
public:

	virtual bool init() = 0;
	virtual bool shutdown() = 0;

	void require();
};