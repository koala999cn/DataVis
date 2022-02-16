#pragma once
#include <atomic>
#include <assert.h>
#include "KmNonCopyable.h"


// 带引用计数的对象接口
class KvRefObject : protected KmNonCopyable
{
protected:
	KvRefObject() : refCount_(1) {}

public:

	virtual ~KvRefObject() {
		assert(refCount_ == 0);
	}


	// 返回false表示this对象已被销毁，增加引用失败
	bool addRef() { 
		if (refCount_ == 0) // this was destroyed
			return false;

		refCount_++;
		return true;
	}


	// 返回true表示此次调用导致引用计数清零，并销毁this对象 
	bool release() { 
		if (--refCount_ == 0) {
			delete this;
			return true;
		}

		return false;
	}
	

	int refCount() const { 
		return refCount_;
	}

private:
	std::atomic_int refCount_; 
};

