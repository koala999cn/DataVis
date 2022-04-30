#pragma once
#include "kDsp.h"


// ÂË²¨Æ÷³éÏó½Ó¿Ú

class KvFilter
{
public:
	virtual ~KvFilter() {}

	virtual kIndex taps() const = 0;

	virtual void apply(const kReal* in, kReal* out, kIndex N) = 0;

	virtual void apply(kReal* inout, kIndex N) {
		apply(inout, inout, N);
	}

	virtual void flush(kReal out[/*delay*/]) {
		for (kIndex i = 0; i < taps(); i++) out[i] = 0;
		apply(out, taps());
	};
};
