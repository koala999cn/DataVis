#pragma once
#include "KvImModalWindow.h"


class KcImExprEditor : public KvImModalWindow
{
public:
	KcImExprEditor(std::string& expr);

private:

	void updateImpl_() override;

private:
	std::string expr_;
};
