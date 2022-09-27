#pragma once
#include "KtUniObject.h"


// 程序模块的抽象接口，提供初始化接口
class KvModule : public KtUniObject<KvModule>
{
public:

	using super_ = KtUniObject<KvModule>;
	using super_::super_;


	virtual bool initialize() = 0;

	// 当程序的所有模块都成功初始化，调用该接口方法
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
