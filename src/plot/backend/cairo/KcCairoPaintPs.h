#pragma once
#include "KvCairoPaint.h"
#include <string_view>

class KcCairoPaintPs : public KvCairoPaint
{
public:
	// ������cairo������
	KcCairoPaintPs(const std::string_view& path);

	// ����cairo������
	KcCairoPaintPs(const std::string_view& path, const rect_t& canvas);

private:
	void* createSurface_() const override;

private:
	std::string pspath_;
};