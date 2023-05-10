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
	// 若empty, 使用dir模式，否则file模式
	KcActionShowFileDialog(KeType type, const std::string_view& title,
		const std::string_view& filter, const std::string_view& dir = "");

	bool trigger() override;

	void update() override;

	// 获取用户选择的打开或保存路径结果
	const std::string& result() const {
		return result_;
	}

	const std::string& filter() const { return filter_; }
	std::string& filter() { return filter_; }

private:
	std::string key_; // 自动生成，内部使用
	KeType type_;
	std::string filter_;
	std::string dir_; // 起始目录
	std::string result_;
};
