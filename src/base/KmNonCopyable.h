#pragma once


class KmNonCopyable
{
protected:
	KmNonCopyable() = default;

	KmNonCopyable(const KmNonCopyable&) = delete;
	KmNonCopyable(KmNonCopyable&&) = delete;

	KmNonCopyable& operator=(const KmNonCopyable&) = delete;
	KmNonCopyable& operator=(KmNonCopyable&&) = delete;
};