#pragma once
#include "KvImModalWindow.h"
#include <memory>

class KcPvData;
class KvData;

class KcImExprEditor : public KvImModalWindow
{
	using super_ = KvImModalWindow;

public:
	KcImExprEditor(std::string* text, KcPvData* pvData);

	int flags() const override;

	void onClose(bool clicked) override;

private:

	void updateImpl_() override;

	std::shared_ptr<KvData> compile_();

private:
	std::string origText_;
	std::string* exprText_;
	KcPvData* pvData_;
};
