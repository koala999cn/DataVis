#include "KgImMenu.h"
#include "imgui.h"
#include <assert.h>


KgImMenu::KgImMenu(const std::string_view label)
	: label_(label)
{

}


KgImMenu::KpItem& KgImMenu::addItem(const std::string_view label, std::function<void()> handler, bool selected)
{
	KpItem item;
	item.label = label;
	item.handler = handler;
	item.selected = selected;
	items_.push_back(item);
	return items_.back();
}


void KgImMenu::addSeperator()
{
	KpItem item;
	item.label = "";
	item.handler = nullptr;
	item.selected = false;
	items_.push_back(item);
}


void KgImMenu::apply()
{
	if (ImGui::BeginMenu(label_.c_str())) {
		for (auto& i : items_)
			apply_(i);

		ImGui::EndMenu();
	}
}


void KgImMenu::apply_(KpItem& item)
{
	if (item.subItems.empty()) {
		if (item.label.empty())
			ImGui::Separator();
		else if (ImGui::MenuItem(item.label.c_str(), 0, &item.selected)) {
			if (item.handler)
				item.handler();
		}
	}
	else {
		if (ImGui::BeginMenu(item.label.c_str())) {

			if (item.handler)
				item.handler();

			for (auto& i : item.subItems)
				apply_(i);

			ImGui::EndMenu();
		}
	}
}