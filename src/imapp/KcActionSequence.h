#pragma once
#include "KvAction.h"
#include <vector>
#include <memory>


// ��װaction���У����δ��������е�action��������actionִ����ɣ�������ִ�����
class KcActionSequence : public KvAction
{
public:
	// ���û�ָ��action���ֺ�����
	KcActionSequence(const std::string_view& name, const std::string_view& desc);

	auto desc() const -> const std::string_view override;

	bool trigger() override;

	void update() override;

	void addAction(std::shared_ptr<KvAction> act);

private:
	std::string desc_;
	std::vector<std::shared_ptr<KvAction>> seqs_;
};

