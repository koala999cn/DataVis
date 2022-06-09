#pragma once
#include "KvExcitor.h"
#include "KgRand.h"


class KcGaussExcitor : public KvExcitor
{
public:

	KcGaussExcitor() : mu_(0), sigma_(1) {}

	KcGaussExcitor(kReal mu, kReal sigma) : mu_(mu), sigma_(sigma) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.gauss(mu_, sigma_));
	}

	std::pair<kReal, kReal> range() const override {
		return { mu_ - 6 * sigma_, mu_ + 6 * sigma_ };
	}

private:
	KgRand r_;
	kReal mu_;
	kReal sigma_;
};



class KcExponentExcitor : public KvExcitor
{
public:

	KcExponentExcitor() : beta_(1) {}

	KcExponentExcitor(kReal beta) : beta_(beta) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.exponent(beta_));
	}

	std::pair<kReal, kReal> range() const override {
		return { 0, std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal beta_;
};


class KcLaplaceExcitor : public KvExcitor
{
public:

	KcLaplaceExcitor() : beta_(1) {}

	KcLaplaceExcitor(kReal beta) : beta_(beta) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.laplace(beta_));
	}

	std::pair<kReal, kReal> range() const override {
		return { -std::numeric_limits<kReal>::infinity(), std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal beta_;
};


class KcRayleighExcitor : public KvExcitor
{
public:

	KcRayleighExcitor() : sigma_(1) {}

	KcRayleighExcitor(kReal sigma) : sigma_(sigma) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.rayleigh(sigma_));
	}

	std::pair<kReal, kReal> range() const override {
		return { 0, std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal sigma_;
};


class KcLognormExcitor : public KvExcitor
{
public:

	KcLognormExcitor() : mu_(0), sigma_(1) {}

	KcLognormExcitor(kReal mu, kReal sigma) : mu_(mu), sigma_(sigma) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.lognorm(mu_, sigma_));
	}

	std::pair<kReal, kReal> range() const override {
		return { 0, std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal mu_;
	kReal sigma_;
};


class KcCauchyExcitor : public KvExcitor
{
public:

	KcCauchyExcitor() : alpha_(0), beta_(1) {}

	KcCauchyExcitor(kReal alpha, kReal beta) : alpha_(alpha_), beta_(beta) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.cauchy(alpha_, beta_));
	}

	std::pair<kReal, kReal> range() const override {
		return { -std::numeric_limits<kReal>::infinity(), std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal alpha_;
	kReal beta_;
};


class KcWeibullExcitor : public KvExcitor
{
public:

	KcWeibullExcitor() : alpha_(1), beta_(1) {}

	KcWeibullExcitor(kReal alpha, kReal beta) : alpha_(alpha_), beta_(beta) {}

	kReal pulse() override {
		return static_cast<kReal>(r_.weibull(alpha_, beta_));
	}

	std::pair<kReal, kReal> range() const override {
		return { 0, std::numeric_limits<kReal>::infinity() };
	}

private:
	KgRand r_;
	kReal alpha_;
	kReal beta_;
};
