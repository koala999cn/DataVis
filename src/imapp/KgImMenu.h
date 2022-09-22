#pragma once
#include <string>
#include <functional>


// ��װim�˵�����ʾ�봦��

class KgImMenu
{
public:

	struct KpItem
	{
		std::string label;
		std::function<void()> handler;
		bool selected;
		std::vector<KpItem> subItems;
	};

	KgImMenu(const std::string_view label);

	KpItem& addItem(const std::string_view label, std::function<void()> handler, bool selected = false);

	void addSeperator();

	void apply();

private:

	void apply_(KpItem& item);

private:
	std::string label_;
	std::vector<KpItem> items_;
};

