#pragma once
#include "KtSingleton.h"
#include <vector>
#include <map>

// 基于pango的字体管理器

class KsPangoFontManager
{
public:
	using singleton_type = KtSingleton<KsPangoFontManager, false, true>;
	friend singleton_type;

	static KsPangoFontManager& singleton() {
		return *singleton_type::instance();
	}

	auto families() const { return families_.size(); }

	const std::vector<std::string_view>& listFamilies() const { return families_; }

	std::string_view familyName(int idx) const { return families_[idx]; }

private:
	std::vector<std::string_view> families_;
	std::map<std::string_view, void*> familiesMap_; // family name -> family struct


private:
	KsPangoFontManager();
	~KsPangoFontManager();

	KsPangoFontManager(const KsPangoFontManager& app) = delete;
	void operator=(const KsPangoFontManager& app) = delete;
};
