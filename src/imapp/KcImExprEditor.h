#pragma once
#include "KvImModalWindow.h"
#include <memory>

class KvData;

class KcImExprEditor : public KvImModalWindow
{
	using super_ = KvImModalWindow;

public:
	KcImExprEditor(std::string* text, std::shared_ptr<KvData>* data);

	int flags() const override;

	void onClose(bool clicked) override;

private:

	void updateImpl_() override;

	bool compile_();

private:
	std::string origText_;
	std::string* exprText_;
	std::shared_ptr<KvData>* data_;
};
