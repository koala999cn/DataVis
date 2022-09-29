#pragma once
#include "KvAction.h"
#include <vector>
#include <memory>


// 封装action序列，依次触发序列中的action，当所有action执行完成，该序列执行完成
class KcActionSequence : public KvAction
{
public:
	// 有用户指定action名字和描述
	KcActionSequence(const std::string_view& name, const std::string_view& desc);

	auto desc() const -> const std::string_view override;

	bool trigger() override;

	void update() override;

	void addAction(std::shared_ptr<KvAction> act);

private:
	std::string desc_;
	std::vector<std::shared_ptr<KvAction>> seqs_;
};

