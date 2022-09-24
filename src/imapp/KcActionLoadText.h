#pragma once
#include "KvAction.h"


class KcActionLoadText : public KvAction
{
public:

	explicit KcActionLoadText(const std::string_view& name) 
		: KvAction(name) {}


	auto desc() const -> const std::string_view override;

	bool trigger() override;

	void update() override;

};
