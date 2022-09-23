#include "KcImActionList.h"
#include "imgui.h"
#include "ImFileDialog/ImFileDialog.h"


KcImActionList::KcImActionList(const std::string_view& name)
	: KvImWindow(name)
{

}


void KcImActionList::draw()
{
	drawProvider_();
	drawOperator_();
	drawRenderer_();
}


void KcImActionList::drawProvider_()
{
	// Æ½ÆÌ
	const auto btnSize = ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f };

	if (ImGui::CollapsingHeader("Provider")) {
		if (ImGui::Button("Text File", btnSize)) {
			ifd::FileDialog::Instance().Open(
				"opentext", "Load Text Data", "text file (*.txt;*.cvs){.txt,.cvs},.*");
		}

		if (ifd::FileDialog::Instance().IsDone("opentext")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				std::string res = ifd::FileDialog::Instance().GetResult().u8string();
			}
			ifd::FileDialog::Instance().Close();
		}
	}


}


void KcImActionList::drawOperator_()
{
	if (ImGui::CollapsingHeader("Operator")) {

	}
}


void KcImActionList::drawRenderer_()
{
	if (ImGui::CollapsingHeader("Render")) {

	}
}

