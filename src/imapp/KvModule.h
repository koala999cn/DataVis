#pragma once
#include "KtUniObject.h"


// ����ģ��ĳ���ӿڣ��ṩ��ʼ���ӿ�
class KvModule : public KtUniObject<KvModule>
{
public:

	using super_ = KtUniObject<KvModule>;
	using super_::super_;


	virtual bool initialize() = 0;

	// �����������ģ�鶼�ɹ���ʼ�������øýӿڷ���
	virtual bool postInitialize() { return true; }

	virtual void deinitialize() = 0;


	// TODO:
	/*	enum class KeState
	{
		k_uninitialize, // befor call initialize
		k_initializing, // be calling initialize
		k_initialized, // after called initialize
		k_ready, // after called postInitialize
		k_deinitializing, // be calling deinitialize
		k_deinitialized, // after call deinitialize
		k_failed // some error occured
	};*/
};
