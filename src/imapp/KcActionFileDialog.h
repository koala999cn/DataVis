#pragma once
#include "KvAction.h"
#include <filesystem>


class KcActionFileDialog : public KvAction
{
public:

	enum class KeType { k_open, k_open_multi, k_save };

	// @filter: File filter. syntax: Name1{ .ext1,.ext2 }, Name2{ .ext3,.ext4 }, .*
	// 若empty, 使用dir模式，否则file模式
	KcActionFileDialog(KeType type, const std::string_view& title, 
		const std::string_view& filter, const std::string_view& dir = "");

	auto desc() const -> const std::string_view override;

	bool trigger() override;

	void update() override;

	// 获取用户选择的打开或保存路径结果
	auto result() const -> const std::filesystem::path&;
	auto results() const -> const std::vector<std::filesystem::path>&;

private:
	std::string key_; // 自动生成，内部使用
	KeType type_;
	std::string filter_;
	std::string dir_; // 起始目录
};
