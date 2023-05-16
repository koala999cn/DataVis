#pragma once
#include "KvCairoPaint.h"
#include <string_view>

class KcCairoPaintPdf : public KvCairoPaint
{
public:
	// ������cairo������
	KcCairoPaintPdf(const std::string_view& path);

	// ����cairo������
	KcCairoPaintPdf(const std::string_view& path, const rect_t& canvas);

private:
	void* createSurface_() const override;

private:
	std::string pdfpath_;
};