#pragma once
#include "KvImModalWindow.h"


class KcImExprEditor : public KvImModalWindow
{
public:
	KcImExprEditor(std::string& expr);

	int flags() const override;

private:

	void updateImpl_() override;

private:
	std::string expr_;
};
