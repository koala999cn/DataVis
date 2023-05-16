#pragma once
#include "KvCairoPaint.h"
#include <string_view>

class KcCairoPaintSvg : public KvCairoPaint
{
public:
	// ������cairo������
	KcCairoPaintSvg(const std::string_view& path);

	// ����cairo������
	KcCairoPaintSvg(const std::string_view& path, const rect_t& canvas);

private:
	void* createSurface_() const override;

private:
	std::string svgpath_;
};