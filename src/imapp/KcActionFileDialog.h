#pragma once
#include "KvAction.h"
#include <filesystem>


class KcActionFileDialog : public KvAction
{
public:

	enum class KeType { k_open, k_open_multi, k_save };

	KcActionFileDialog(KeType type, const std::string_view& title, 
		const std::string_view& filter, const std::string_view& dir = "");

	auto desc() const -> const std::string_view override;

	bool trigger() override;

	void update() override;

	// ��ȡ�û�ѡ��Ĵ򿪻򱣴�·�����
	auto result() const -> const std::filesystem::path&;
	auto results() const -> const std::vector<std::filesystem::path>&;

private:
	std::string key_; // �Զ����ɣ��ڲ�ʹ��
	KeType type_;
	std::string filter_;
	std::string dir_; // ��ʼĿ¼
};
