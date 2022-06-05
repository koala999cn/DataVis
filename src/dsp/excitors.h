#pragma once
#include "KvExcitor.h"
#include "KgRand.h"


class KcGaussExcitor : public KvExcitor
{
public:

	KcGaussExcitor()
		: mu_(0), sigma_(1) {}

	KcGaussExcitor(kReal mu, kReal sigma)
		: mu_(mu), sigma_(sigma) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.gaussFast(mu_, sigma_));
	}

private:
	KgRand r_;
	kReal mu_;
	kReal sigma_;
};
