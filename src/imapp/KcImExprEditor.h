#pragma once
#include "KvImModalWindow.h"
#include <memory>
#include <functional>

class KvData;

class KcImExprEditor : public KvImModalWindow
{
	using super_ = KvImModalWindow;
	using handler_t = std::function<void(std::shared_ptr<KvData>, const char*)>;

public:
	KcImExprEditor(const char* text, unsigned dim, handler_t h);

	int flags() const override;

private:

	void updateImpl_() override;

	std::shared_ptr<KvData> compile_();

private:
	std::string text_;
	handler_t handler_;
	unsigned dim_; // 维度限制，0表示无限制
};
