#pragma once
#include "KvImWindow.h"


class KcImPlot1d : public KvImWindow
{
public:

	KcImPlot1d(const std::string_view& name);

private:

	void updateImpl_() override;
};
