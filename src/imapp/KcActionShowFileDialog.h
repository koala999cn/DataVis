#pragma once
#include "KvAction.h"
#include <filesystem>


class KcActionShowFileDialog : public KvAction
{
public:

	enum class KeType { 
		k_open, 
		k_save,
		// k_open_multi
	};

	// @filter: File filter. syntax: Name1{ .ext1,.ext2 }, Name2{ .ext3,.ext4 }, .*
	// ��empty, ʹ��dirģʽ������fileģʽ
	KcActionShowFileDialog(KeType type, const std::string_view& title,
		const std::string_view& filter, const std::string_view& dir = "");

	bool trigger() override;

	void update() override;

	// ��ȡ�û�ѡ��Ĵ򿪻򱣴�·�����
	const std::string& result() const {
		return result_;
	}

	const std::string& filter() const { return filter_; }
	std::string& filter() { return filter_; }

private:
	std::string key_; // �Զ����ɣ��ڲ�ʹ��
	KeType type_;
	std::string filter_;
	std::string dir_; // ��ʼĿ¼
	std::string result_;
};
