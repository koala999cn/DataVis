#include "KvRdPlot.h"
#include "plot/KvPlot.h"
#include "plot/KvPlottable1d.h"
#include "plot/KsThemeManager.h"
#include "plot/KcThemedPlotImpl_.h"
#include "plot/KcCoord2d.h"
#include "plot/KcCoordPlane.h"
#include "plot/KvPaint.h"
#include "plot/KcPlotTitle.h"
#include "plot/KcLegend.h"
#include "plot/KcColorBar.h"
#include "plot/KcLinearTicker.h"
#include "plot/KcLogTicker.h"
#include "plot/KcLabelTicker.h"
#include "plot/KcTimeTicker.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcMonoDiscreted.h"
#include "dsp/KcMonoContinued.h"
#include "dsp/KcSampled1d.h"
#include "dsp/KcSampled2d.h"
#include "imapp/KvImWindow.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KgPipeline.h"
#include "imguix.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "imGuIZMO.quat/imGuIZMOquat.h"
#include "KvNode.h"


KvRdPlot::KvRdPlot(const std::string_view& name, const std::shared_ptr<KvPlot>& plot)
	: KvDataRender(name)
	, plot_(plot)
{
	plot_->setVisible(false);

	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().registerWindow(imWindow);
}


KvRdPlot::~KvRdPlot()
{
	auto imWindow = std::dynamic_pointer_cast<KvImWindow>(plot_);
	if (imWindow)
		KsImApp::singleton().windowManager().releaseWindow(imWindow);

	plot_ = nullptr;
}


void KvRdPlot::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
	assert(prov);

	// TODO: 
	if (prov->dataStamp(outPort->index()) < KsImApp::singleton().pipeline().frameIndex()) // 数据无变化
		return;

	auto data = prov->fetchData(outPort->index());
	if (data == nullptr)
		return;

	auto r = port2Plts_.equal_range(outPort->id());
	assert(r.first != r.second);

	auto numPlts = std::distance(r.first, r.second);
	
	if (prov->isStream(outPort->index())) { // shifting the data

		assert(prov->isSampled(outPort->index()) || 
			prov->isArray(outPort->index())); // TODO: other types

		auto plt1d = dynamic_cast<KvPlottable1d*>(r.first->second);
		auto xaxis = plt1d ? plt1d->dimAxis(0) : 0;
		unsigned nx = -1;
		if (xaxis > data->dim()) {
			streamData_[outPort].reset();
		}
		else {
			// TODO: 考虑分离坐标轴
			auto xrange = plot_->coord().upper()[xaxis] - plot_->coord().lower()[xaxis];
			auto samp = std::dynamic_pointer_cast<KvSampled>(data);
			assert(samp && samp->step(0) > 0);
			nx = xrange / samp->step(0);
		}

		streamData_[outPort] = streaming_(streamData_[outPort], data, nx);
		data = streamData_[outPort];
	}

	//bool autoRange = r.first->second->empty();

	if (data->channels() == 1 || numPlts == 1) {
		r.first->second->setData(data);
	}
	else {
		assert(data->channels() == numPlts);

		kIndex ch(0);

		if (data->isDiscreted()) {
			auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
			for (auto i = r.first; i != r.second; i++) {
				assert(i->first == outPort->id());
				i->second->setData(std::make_shared<KcMonoDiscreted>(disc, ch++));
			}
		}
		else {
			auto cont = std::dynamic_pointer_cast<KvContinued>(data);
			for (auto i = r.first; i != r.second; i++) {
				assert(i->first == outPort->id());
				i->second->setData(std::make_shared<KcMonoContinued>(cont, ch++));
			}
		}
	}
}


void KvRdPlot::fitRange_()
{
	typename KvCoord::point3
		lower(std::numeric_limits<typename KvCoord::float_t>::max()),
		upper(std::numeric_limits<typename KvCoord::float_t>::lowest());

	auto inputs = KsImApp::singleton().pipeline().getInputs(id(), 0);
	for (auto node : inputs) {
		auto port = dynamic_cast<KcPortNode*>(node);
		assert(port);

		auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());
		assert(prov);

		auto plts = port2Plts_.equal_range(port->id());
		auto dim = plts.first->second->odim(); // TODO: 考虑分离坐标轴
		if (dim == 0) dim = prov->dim(port->index());
		std::vector<unsigned> dims(dim + 1);
		for (unsigned i = 0; i <= dim; i++)
			dims[i] = i;
		auto plt1d = dynamic_cast<KvPlottable1d*>(plts.first->second);
		if (plt1d) {
			dims[0] = plt1d->xdim();
			dims[1] = plt1d->ydim();
			if (dim > 1)
				dims[2] = plt1d->zdim();
		}

		for (unsigned i = 0; i <= dim; i++) {
			auto r = prov->range(port->index(), dims[i]);
			KuMath::uniteRange(lower[i], upper[i], r.low(), r.high());
		}

		// NB: 此处合并plottable的aabb，否则很多内部作了偏移的plottable不能正确显示（如热图，arrange模式等）
		auto box = plts.first->second->boundingBox();
		for (unsigned i = 0; i < 3; i++)
			KuMath::uniteRange(lower[i], upper[i], box.lower()[i], box.upper()[i]);
	}

	if (lower.z() == std::numeric_limits<typename KvCoord::float_t>::max()) // 输入全是二维数据
		lower.z() = upper.z() = 0;

	plot_->coord().setExtents(lower, upper);
}


bool KvRdPlot::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);
	assert(port2Plts_.count(from->id()) == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	return super_::onNewLink(from, to); 
}


void KvRdPlot::onDelLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(from->id());
	if (r.first != r.second) {
		for (auto i = r.first; i != r.second; i++)
			plot_->removePlottable(i->second);
		port2Plts_.erase(from->id());
	}

	if (prov->isStream(from->index()))
		streamData_.erase(from);
}


bool KvRdPlot::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (!super_::onStartPipeline(ins))
		return false;

	plot_->removeAllPlottables();
	port2Plts_.clear(); 
	streamData_.clear();

	if (!ins.empty()) {
		// 根据输入配置plot

		for (unsigned i = 0; i < ins.size(); i++) {
			auto port = ins[i].second;
			auto portIdx = port->index();
			auto node = port->parent().lock();
			if (!node) continue;

			auto prov = dynamic_cast<KvDataProvider*>(node.get());
			assert(prov);

			auto plts = createPlottable_(port);
			if (plts.empty())
				return false;

			for (auto plt : plts) {
				plot_->addPlottable(plt);
				port2Plts_.insert(std::make_pair(port->id(), plt));
			}

			if (prov->isStream(portIdx)) {
				if (prov->dim(portIdx) == 1)
					streamData_[port] = std::make_shared<KcSampled1d>(
						prov->step(portIdx, 0),
						prov->channels(portIdx));
				else if (prov->dim(portIdx) == 2) {
					auto data = std::make_shared<KcSampled2d>(
						prov->step(portIdx, 0),
						prov->step(portIdx, 1),
						prov->channels(portIdx));

					data->resize(0, prov->size(portIdx, 1));
					streamData_[port] = data;
				}
			}
		}
		
		// update theme
		updateTheme_();

		plot_->setVisible(true);
	}

	return true;
}


bool KvRdPlot::hasOutput_() const
{
	for (unsigned i = 0; i < plot_->plottableCount(); i++) {
		auto plt = plot_->plottableAt(i);
		if (plt->idata() != plt->odata())
			return true;
	}

	return false;
}


void KvRdPlot::onNewFrame(int frameIdx)
{
	// 用户调整plot的维度映射之后，range也跟着变化，但目前没有好的机制检测到这种变化
	// 所以每帧检测并更新range
	if (autoRange_) {
		if (!hasOutput_()) // 当plottable输入输出不一致时，fitRange_将失效，此时调用fitData
			fitRange_();
		else
			plot_->fitData();
	}
}


void KvRdPlot::showPropertySet()
{
	super_::showPropertySet(); // show the name property

	ImGui::Separator();
	showPlotProperty_();

	ImGui::Separator();
	showThemeProperty_();

	ImGui::Separator();
	showCoordProperty_();

	if (plot_->plottableCount() > 0) {
		ImGui::Separator();
		showPlottableProperty_();
	}

	ImGui::Separator();
	showTitleProperty_();

	if (plot_->legend()->itemCount() > 0) {
		ImGui::Separator();
		showLegendProperty_();
	}

	if (plot_->colorbarCount() > 0) {
		ImGui::Separator();
		showColorBarProperty_();
	}
}


void KvRdPlot::showPlotProperty_()
{
	if (ImGuiX::treePush("Plot", false)) {
		bool vis = plot_->visible();
		auto name = plot_->name();
		if (ImGuiX::cbInputText("Title", &vis, &name)) {
			plot_->setVisible(vis);
			plot_->setName(name);
		}

		ImGui::ColorEdit4("Background", plot_->bkgndBrush().color);

		auto rc = plot_->outterRect();
		int width = std::round(rc.width()), height = std::round(rc.height());
		ImGui::BeginDisabled();
		ImGui::DragIntRange2("Canvas", &width, &height);
		ImGui::EndDisabled();

		ImGuiX::margins("Margins", plot_->margins());

		auto& paint = plot_->paint();
		bool anti = paint.antialiasing();
		if (ImGui::Checkbox("Antialiasing", &anti))
			paint.enableAntialiasing(anti);

		bool depth = paint.depthTest();
		if (ImGui::Checkbox("Depth Test", &depth))
			paint.enableDepthTest(depth);

		bool lighting = paint.lighting();
		if (ImGui::Checkbox("Lighting", &lighting))
			paint.enableLighting(lighting);

		auto ambient = paint.ambientColor();
		if (ImGui::ColorEdit3("Ambient Color", ambient.data()))
			paint.setAmbientColor(ambient);

		auto lightColor = paint.lightColor();
		if (ImGui::ColorEdit3("Light Color", lightColor.data()))
			paint.setLightColor(lightColor);

		auto lightDir = paint.lightDirection();
		vec3 fDir(lightDir.x(), lightDir.y(), lightDir.z());
		if (fDir.x == -1 && fDir.y == 0 && fDir.z == 0)
			fDir.y = 0.00001; // NOTE: 防止出现(-1, 0, 0)， 否则方向绘制不正确

		bool dirUpdated(false);
		dirUpdated = ImGui::gizmo3D("Direction", fDir);
		ImGui::SameLine();
		dirUpdated |= ImGui::DragFloat3("##", (float*)&fDir, 0.01);
		if (dirUpdated)
			paint.setLightDirection(point3d(fDir.x, fDir.y, fDir.z));

		auto specColor = paint.specularColor();
		if (ImGui::ColorEdit3("Specular Color", specColor.data()))
			paint.setSpecularColor(specColor);

		float shininess = paint.shininess();
		if (ImGui::SliderFloat("Shininess", &shininess, 1, 99))
			paint.setShininess(shininess);

		ImGui::Checkbox("Show Layout Rect", &plot_->showLayoutRect());

		ImGuiX::layout("Layout", plot_.get());

		ImGui::BeginDisabled(working_());
		ImGui::Checkbox("Split Channels", &splitChannels_);
		ImGui::EndDisabled();

		ImGuiX::treePop();
	}
}


void KvRdPlot::showThemeProperty_()
{
	auto& themeMgr = KsThemeManager::singleton();
	if (themeMgr.empty())
		return;

	if (!ImGuiX::treePush("Design", false))
		return;

	auto groups = themeMgr.listGroups();
	groups.insert(groups.cbegin(), ""); // insert the global group
	static const char* labels[] = { "Theme", "Layout", "Canvas", "Palette" };

	for (int i = 0; i < 4; i++) {
		if (themeMgr.isEmpty(i))
			continue;

		if (!ImGui::BeginCombo(labels[i], curTheme_[i].second.c_str(), ImGuiComboFlags_HeightLarge))
			continue;

		for (unsigned g = 0; g < groups.size(); g++) {

			auto names = themeMgr.listNames(i, groups[g]);
			if (names.empty())
				continue;

			if (!groups[g].empty() && !ImGui::TreeNodeEx(groups[g].c_str()))
			    continue;
		
			for (auto& name : names) {
				if (ImGui::Selectable(name.c_str(), name == curTheme_[i].second && groups[g] == curTheme_[i].first)) {
					curTheme_[i].second = name;
					curTheme_[i].first = groups[g];
					onThemeChanged(i);
				}

				if (name == curTheme_[i].second && groups[g] == curTheme_[i].first)
					ImGui::SetItemDefaultFocus();

				// 绘制色带
				if (i == KsThemeManager::k_canvas) {
					ImGui::SameLine();
					auto canvas = KsThemeManager::singleton().getCanvas(groups[g], name);
					ImGuiX::drawColorBar(canvas, ImGui::GetCursorScreenPos(),
						{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() }, 1);
				}
				else if (i == KsThemeManager::k_palette) {
					ImGui::SameLine();
					std::vector<color4f> majors, minors;
					KsThemeManager::singleton().getPalette(groups[g], name, majors, minors);
					ImGuiX::drawColorBar(majors, ImGui::GetCursorScreenPos(),
						{ 6, ImGui::GetTextLineHeight() }, 2);
				}
			}

			if (!groups[g].empty())
				ImGui::TreePop();
		}

		ImGui::EndCombo();
	}

	ImGuiX::treePop();
}


void KvRdPlot::showCoordProperty_()
{
	if (ImGuiX::treePush("Coordinate System", false)) {

		auto& coord = plot_->coord();
		if (ImGui::Checkbox("Auto Range", &autoRange_) && autoRange_)
			fitRange_();
		ImGui::SameLine();
		if (ImGui::Button("Fit Data")) {
			plot_->fitData();
			autoRange_ = false;
		}

		auto lower = coord.lower();
		auto upper = coord.upper();
		auto speed = (upper - lower) * 0.001;
		for (unsigned i = 0; i < speed.size(); i++)
			if (speed.at(i) == 0) speed.at(i) = 1;
		static const char* axisName[] = { "X Range", "Y Range", "Z Range" };
		for (char i = 0; i < 3; i++) {
			double val[2] = { lower[i], upper[i] };
			if (ImGui::DragScalarN(axisName[i], ImGuiDataType_Double, val, 2, speed[i])
				&& val[1] > val[0]) {
				lower[i] = val[0], upper[i] = val[1];
				coord.setExtents(lower, upper);
				autoRange_ = false;

				// 第i维度的主坐标轴range发生变化
				auto tic = coord.defaultAxis(i)->ticker();
				if (tic->map(lower[i]) != lower[i] || tic->map(upper[i]) != upper[i]) { // 非线性域坐标轴，须重构plt渲染对象
					for (unsigned i = 0; i < plot_->plottableCount(); i++) {
						auto plt = plot_->plottableAt(i);
						if (plt->axis(i)->main())
							plt->setDataChanged(false);
					}
				}
			}
		}

		const char* label[] = { "Invert X", "Invert Y", "Invert Z" };
		for (int i = 0; i < plot_->dim(); i++) {
			bool inv = coord.axisInversed(i);
			if (ImGui::Checkbox(label[i], &inv))
				coord.inverseAxis(i, inv);
		}

		const char* swapStr[] = { "No Swap", "Swap XY", "Swap XZ", "Swap YZ" };
		if (ImGui::BeginCombo("Swap Axes", swapStr[coord.axisSwapped()])) {
			unsigned c = plot_->dim() > 2 ? std::size(swapStr) : plot_->dim();
			for (unsigned i = 0; i < c; i++)
				if (ImGui::Selectable(swapStr[i], i == coord.axisSwapped()))
					coord.swapAxis(KvCoord::KeAxisSwapStatus(i));

			ImGui::EndCombo();
		}

		if (ImGuiX::treePush("Axes", false)) {
			plot_->coord().forAxis([this](KcAxis& axis) {
				auto tic = axis.ticker();
				if (axis.main() && showAxisProperty_(axis)) { // 仅显示主坐标轴
					for (unsigned i = 0; i < plot_->plottableCount(); i++) {
						auto plt = plot_->plottableAt(i);
						if (plt->axis(axis.dim()).get() == &axis)
							plt->setDataChanged(false);
					}

					if (axis.ticker() != tic) { // ticker发生变化，须同步其他同一维度的主坐标轴
						plot_->coord().forAxis([&axis](KcAxis& ax) {
							if (ax.main() && ax.dim() == axis.dim() && &ax != &axis)
								ax.setTicker(axis.ticker()); // TODO: ticker不共享
							return true;
							});
					}
				}
				return true;
				});
			ImGuiX::treePop();
		}

		if (ImGuiX::treePush("Planes", false)) {
			plot_->coord().forPlane([this](KcCoordPlane& plane) {
				showPlaneProperty_(plane);
				return true;
				});
			ImGuiX::treePop();
		}

		ImGuiX::treePop();
	}
}


namespace kPrivate
{
	void tickContext(KcAxis::KpTickContext& cxt, bool subtick)
	{
		ImGuiX::pen(cxt, false, true); // no style. tick始终使用solid线条

		ImGui::PushID(&cxt);

		ImGui::SliderFloat("Length", &cxt.length, 0, 25, "%.1f px");

		if (!subtick) {
			const static char* side[] = { "inside", "outside", "bothside" };
			if (ImGui::BeginCombo("Side", side[cxt.side])) {
				for (unsigned i = 0; i < std::size(side); i++)
					if (ImGui::Selectable(side[i], i == cxt.side))
						cxt.side = KcAxis::KeTickSide(i);
				ImGui::EndCombo();
			}

			float yaw = KuMath::rad2Deg(cxt.yaw);
			if (ImGui::SliderFloat("Yaw", &yaw, -90, 90, "%.f deg"))
				cxt.yaw = KuMath::deg2Rad(yaw);

			float pitch = KuMath::rad2Deg(cxt.pitch);
			if (ImGui::SliderFloat("Pitch", &pitch, -90, 90, "%.f deg"))
				cxt.pitch = KuMath::deg2Rad(pitch);
		}

		ImGui::PopID();
	}

	void textContext(KcAxis::KpTextContext& cxt, bool showYawAndPitch)
	{
		ImGui::PushID(&cxt);

		ImGui::Checkbox("Billboard", &cxt.billboard);

		static const char* layouts[] = {
			"standard",
			"upside down",
			"vertical left",
			"vertical rigth"
		};
		if (ImGui::BeginCombo("Layout", layouts[cxt.layout])) {
			for (unsigned i = 0; i < std::size(layouts); i++)
				if (ImGui::Selectable(layouts[i], i == cxt.layout))
					cxt.layout = KcAxis::KeTextLayout(i);
			ImGui::EndCombo();
		}

		ImGui::ColorEdit4("Color##label", cxt.color);

		ImGuiX::font(cxt.font);

		if (showYawAndPitch) {
			float yaw = KuMath::rad2Deg(cxt.yaw);
			if (ImGui::SliderFloat("Yaw", &yaw, -90, 90, "%.f deg"))
				cxt.yaw = KuMath::deg2Rad(yaw);

			float pitch = KuMath::rad2Deg(cxt.pitch);
			if (ImGui::SliderFloat("Pitch", &pitch, -90, 90, "%.f deg"))
				cxt.pitch = KuMath::deg2Rad(pitch);
		}

		ImGui::PopID();
	}

	static int tickerType(const KvTicker* t)
	{
		if (dynamic_cast<const KcLogTicker*>(t))
			return 1;
		else if (dynamic_cast<const KcLabelTicker*>(t))
			return 2;
		else if (dynamic_cast<const KcTimeTicker*>(t))
			return 3;
		return 0;
	}

	static std::shared_ptr<KvTicker> newTicker(int type)
	{
		if (type == 1)
			return std::make_shared<KcLogTicker>();
		else if (type == 2)
			return std::make_shared<KcLabelTicker>();
		else if (type == 3)
			return std::make_shared<KcTimeTicker>();
		return std::make_shared<KcLinearTicker>();
	}


	static void tickerSpecific(KcAxis& ax)
	{
		if (std::dynamic_pointer_cast<KcTimeTicker>(ax.ticker())) {
			auto time = std::dynamic_pointer_cast<KcTimeTicker>(ax.ticker());
			ImGui::Combo("Format", &time->format(), time->formatTextList(), time->formatCount());
		}
		else if (std::dynamic_pointer_cast<KvNumericTicker>(ax.ticker())) {
			auto numeric = std::dynamic_pointer_cast<KvNumericTicker>(ax.ticker());
			if (ImGuiX::treePush("Formatter", false)) {
				ImGuiX::format(numeric->formatter());
				ImGuiX::treePop();
			}
		}
		else if (std::dynamic_pointer_cast<KcLabelTicker>(ax.ticker())) {
			auto label = std::dynamic_pointer_cast<KcLabelTicker>(ax.ticker());
			if (ImGuiX::treePush("Text(s)", false)) {
				label->update(ax.lower(), ax.upper(), true);

				for (unsigned i = 0; i < label->ticksTotal(); i++) {

					ImGui::PushID(i);

					auto w = ImGui::CalcItemWidth();
					ImGui::PushItemWidth(0.5 * (w - ImGui::GetStyle().ItemSpacing.x));

					float tick = label->tick(i);
					if (ImGui::DragFloat("##", &tick, ax.length() * 0.001, ax.lower(), ax.upper()))
						label->setTick(i, tick);
		
					ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);

					auto text = label->label(i);
					std::string id = "Label"; id += std::to_string(i + 1);
					if (ImGui::InputText(id.c_str(), &text))
						label->setLabel(i, text);

					ImGui::PopItemWidth();
					ImGui::PopID();
				}
				ImGuiX::treePop();
			}
		}
	}


	bool axis(const char* label, KcAxis& ax)
	{
		bool open = false;
		auto vis = ax.visible();
		if (ImGuiX::cbTreePush(label, &vis, &open))
			ax.setVisible(vis);
		if (!open) return false;

		bool dataChanged(false);

		ImGui::PushID(&ax);

		static const char* typeStr[] = { "linear", "log", "labels", "time"};
		int type = tickerType(ax.ticker().get());
		if (ImGui::Combo("Type", &type, typeStr, std::size(typeStr))) {
			auto newtic = newTicker(type);
			newtic->ticksExpected() = ax.ticker()->ticksExpected();
			ax.setTicker(newtic);
			dataChanged = true;
		}

		if (!ax.main()) { // 对于分离坐标轴，显示range设置
			float lower = ax.lower(), upper = ax.upper();
			auto speed = ax.length() * 0.005;
			if (ImGui::DragFloatRange2("Range", &lower, &upper, speed) && lower < upper) {
				ax.setRange(lower, upper);
				if (ax.ticker()->map(lower) != lower ||
					ax.ticker()->map(upper) != upper) // 非线性ticker，须重构数据
					dataChanged = true;
			}
		}

		auto& off = ax.offset();
		ImGui::DragScalarN("Offset", ImGuiDataType_Double, off.data(), 2);

		open = false;
		ImGuiX::cbTreePush("Baseline", &ax.showBaseline(), &open);
		if (open) {
			ImGuiX::pen(ax.baselineContext(), true, true);
			ImGuiX::cbTreePop();
		}

		open = false;
		ImGuiX::cbTreePush("Title", &ax.showTitle(), &open);
		if (open) {
			ImGui::InputText("Text", &ax.title());
			kPrivate::textContext(ax.titleContext(), false);
			ImGui::DragFloat("Padding", &ax.titlePadding(), 1, 0, 20, "%.f px");
			ImGuiX::cbTreePop();
		}

		open = false;
		ImGuiX::cbTreePush("Tick", &ax.showTick(), &open);
		if (open) {
			int ticks = ax.ticker()->ticksExpected();
			if (ImGui::SliderInt("Count", &ticks, 1, 16)) 
				ax.ticker()->ticksExpected() = ticks;

			kPrivate::tickContext(ax.tickContext(), false);
			ImGuiX::cbTreePop();
		}

		open = false;
		ImGuiX::cbTreePush("Subtick", &ax.showSubtick(), &open);
		if (open) {
			int subticks = ax.ticker()->subticksExpected();
			if (ImGui::DragInt("Count", &subticks, 1, 0, 1024))
				ax.ticker()->subticksExpected() = subticks;

			kPrivate::tickContext(ax.subtickContext(), true);
			ImGuiX::cbTreePop();
		}

		open = false;
		ImGuiX::cbTreePush("Label", &ax.showLabel(), &open);
		if (open) {
			kPrivate::textContext(ax.labelContext(), true);
			ImGui::DragFloat("Padding", &ax.labelPadding(), 1, 0, 20, "%.f px");
			ImGuiX::cbTreePop();
		}

		tickerSpecific(ax);

		ImGui::PopID();
		ImGuiX::cbTreePop();

		return dataChanged;
	}

	void gridMode(int& mode)
	{
		static const char* modeName[] = {
			"horizontal only",
			"vertical only",
			"both"
		};

		if (ImGui::BeginCombo("Mode", modeName[mode - 1])) {
			for (unsigned i = 0; i < std::size(modeName); i++)
				if (ImGui::Selectable(modeName[i], i == mode - 1))
					mode = i + 1;
			ImGui::EndCombo();
		}
	}
}

bool KvRdPlot::showAxisProperty_(KcAxis& axis)
{
	static const char* name[] = { "X", "Y", "Z" };
	static const char* loc[] = {
		"near-left",
		"near-right",
		"near-bottom",
		"near-top",

		"far-left",
		"far-right",
		"far-bottom",
		"far-top",

		"floor-left",
		"floor-right",
		"ceil-left",
		"ceil-right",
	};

	std::string label = name[axis.dim()]; label += " ["; label += loc[axis.typeReal()]; label += "]";
	return kPrivate::axis(label.c_str(), axis);
}


void KvRdPlot::showPlaneProperty_(KcCoordPlane& plane)
{
	static const char* loc[] = {
		"X-Y Back",
		"X-Y Front",
		"Y-Z Left",
		"Y-Z Right",
		"X-Z Ceil",
		"X-Z Floor"
	};

	bool open = false;
	auto vis = plane.visible();
	
	if (plot_->dim() > 2) {
		if (ImGuiX::cbTreePush(loc[plane.type()], &vis, &open))
			plane.setVisible(vis);
		if (!open) return;
	}
	else {
		if (ImGui::Checkbox("Show", &vis))
			plane.setVisible(vis);
	}

	ImGui::PushID(&plane);

	if (ImGuiX::treePush("Background", false)) {
		ImGuiX::brush(plane.background(), true);
		ImGuiX::treePop();
	}

	open = false;
	ImGuiX::cbTreePush("Major Grid", &plane.majorVisible(), &open);
	if (open) {
		kPrivate::gridMode(plane.majorMode());
		ImGuiX::pen(plane.majorLine(), true, true);
		ImGuiX::cbTreePop();
	}

	open = false;
	ImGuiX::cbTreePush("Minor Grid", &plane.minorVisible(), &open);
	if (open) {
		kPrivate::gridMode(plane.minorMode());
		ImGuiX::pen(plane.minorLine(), true, true);
		ImGuiX::cbTreePop();
	}

	ImGui::PopID();

	if (plot_->dim() > 2) 
	    ImGuiX::cbTreePop();
}


namespace kPrivate
{
	template<typename T>
	KtMargins<T> rect2Margins(const KtAABB<T, 2>& rc) 
	{
		return { rc.lower().x(), rc.lower().y(), rc.upper().x(), rc.upper().y() };
	}

	template<typename T>
	KtAABB<T, 2> margins2Rect(const KtMargins<T>& m)
	{
		KtAABB<T, 2> rc;
		rc.lower() = { m.left(), m.top() }; // NB: 此处不能使用构造函数，否则会自动调整大小
		rc.upper() = { m.right(), m.bottom() };
		return rc;
	}
}


void KvRdPlot::showTitleProperty_()
{
	if (!ImGuiX::treePush("Title", false))
		return;

	auto title = plot_->title();
	ImGui::PushID(title);

	auto vis = title->visible();
	auto name = title->name();
	if (ImGuiX::cbInputText("Text", &vis, &name)) {
		title->setVisible(vis);
		title->setName(name);
	}

	ImGui::ColorEdit4("Color", title->color());

	showDecoratorAlignedProperty_(title);

	if (ImGuiX::treePush("Font", false)) {
		ImGuiX::font(title->font());
		ImGuiX::treePop();
	}

	ImGui::PopID();
	ImGuiX::cbTreePop();
}


void KvRdPlot::showLegendProperty_()
{
	if (!ImGuiX::treePush("Legend", false))
	    return;

	auto legend = plot_->legend();
	ImGui::PushID(legend);

	auto vis = legend->visible();
	if (ImGui::Checkbox("Show", &vis))
		legend->setVisible(vis);

	showDecoratorAlignedProperty_(legend);

	if (ImGuiX::treePush("Layout", false)) {

		ImGui::DragFloat2("Icon Size", legend->iconSize(), 1, 1, 36, "%.f");
		ImGui::DragFloat2("Item Spacing", legend->itemSpacing(), 1, 0, 32, "%.f");
		ImGui::DragFloat("Icon Text Padding", &legend->iconTextPadding(), 1, 0, 32, "%.f");

		ImGuiX::margins("Inner Margins", legend->innerMargins());

		ImGui::ColorEdit4("Text Color", legend->textColor());
		ImGui::SliderInt("Warps", &legend->warps(), 0, legend->itemCount());
		ImGui::Checkbox("Row Major", &legend->rowMajor());

		ImGuiX::treePop();
	}
	
	ImGuiX::font(legend->font());

	ImGui::PopID();
	ImGuiX::cbTreePop();
}


void KvRdPlot::showColorBarProperty_()
{
	if (!ImGuiX::treePush("ColorBar(s)", false))
		return;

	for (unsigned i = 0; i < plot_->colorbarCount(); i++) {
		auto colorbar = plot_->colorbarAt(i);

		ImGui::PushID(colorbar);

		bool open(false);
		auto vis = colorbar->visible();
		if (ImGuiX::cbTreePush(colorbar->name().c_str(), &vis, &open))
			colorbar->setVisible(vis);

		if (open) {

			showDecoratorAlignedProperty_(colorbar);

			ImGui::DragFloat("Bar Width", &colorbar->barWidth(), 1, 1, 64, "%.f");

			ImGui::DragFloat("Bar Length", &colorbar->barLength(), 1, 0, 1024, "%.f");

			open = false;
			ImGuiX::cbTreePush("Bar Border", &colorbar->showBarBorder(), &open);
			if (open) {
				ImGuiX::pen(colorbar->barPen(), true, true);
				ImGuiX::cbTreePop();
			}

			kPrivate::axis("Axis", colorbar->axis());

			ImGuiX::cbTreePop();
		}

		ImGui::PopID();
	}

	ImGuiX::cbTreePop();
}


void KvRdPlot::showDecoratorProperty_(KvDecorator* deco)
{
	bool open = false;
	ImGuiX::cbTreePush("Border", &deco->showBorder(), &open);
	if (open) {
		ImGuiX::pen(deco->borderPen(), true, true);
		ImGuiX::cbTreePop();
	}

	open = false;
	ImGuiX::cbTreePush("Background", &deco->showBkgnd(), &open);
	if (open) {
		ImGuiX::brush(deco->bkgndBrush(), true);
		ImGuiX::cbTreePop();
	}
}


void KvRdPlot::showDecoratorAlignedProperty_(KvDecoratorAligned* deco)
{
	static const char* borderMode[] = {
		"outter", "margins", "inner"
	};
	int mode = deco->borderMode();
	if (ImGui::Combo("Border Mode", &mode, borderMode, std::size(borderMode)))
		deco->borderMode() = (KvDecoratorAligned::KeBorderMode)mode;

	ImGuiX::margins("Margins", deco->margins());

	ImGuiX::alignment("Location", deco->location(), true);

	showDecoratorProperty_(deco);
}


void KvRdPlot::showPlottableProperty_()
{
	if (!ImGuiX::treePush("Plottable(s)", false))
		return;

	for (unsigned idx = 0; idx < plot_->plottableCount(); idx++) {

		auto plt = plot_->plottableAt(idx);

		bool open(false);
		auto vis = plt->visible();
		auto name = plt->name();
		ImGui::PushID(plot_.get() + 1 + idx); // NB: 此处不压入plt，否则变换plt类型时属性状态会重置（因为plt不同了）
		if (ImGuiX::cbiTreePush("##Node", &vis, &name, &open)) {
			plt->setVisible(vis);
			plt->setName(name);
		}

		// 紧跟其后绘制主色块（不换行），提供一个修改主色的快捷通道
		std::vector<color4f> majors(plt->majorColors());
		for (unsigned i = 0; i < plt->majorColors(); i++)
			majors[i] = plt->majorColor(i);
		if (!majors.empty()) {
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			if (ImGuiX::multiColorsEdit(nullptr, (ImVec4*)majors.data(), majors.size(), false))
				plt->setMajorColors(majors);
		}

		if (open) {

			// NB: 若showPlottableBasicProperty_返回true，表示plottable的类型改变，此时plt已失效，略过后续属性显示
			if (!plt->empty() && !showPlottableBasicProperty_(idx, plt)) {
	
				showPlottableArrangeProperty_(plt);
				showPlottableColoringProperty_(plt);
				showPlottableDefaultZProperty_(plt);
				showPlottableSplitAxesProperty_(plt);
				showPlottableSpecificProperty_(plt);

				// for debug
				ImGui::LabelText("Render Objects", "%d reused / %d total(s)", plt->objectsReused(), plt->objectCount());
			}

			ImGuiX::cbiTreePop();
		}

		ImGui::PopID();
	}

	ImGuiX::treePop();
}


bool KvRdPlot::showPlottableBasicProperty_(unsigned idx, KvPlottable* plt)
{
	assert(plot_->plottableAt(idx) == plt);

	bool typeChanged = false;
	int type = plottableType_(plt);
	auto data = plt->odata();
	assert(data);

	if (ImGui::BeginCombo("Type", plottableTypeStr_(type))) {

		for (int i = 0; i < supportPlottableTypes_(); i++) {
			int flags = plottableMatchData_(i, *data) ? 0 : ImGuiSelectableFlags_Disabled;
			if (ImGui::Selectable(plottableTypeStr_(i), i == type, flags)
				&& i != type) {
				auto oldPlt = plt;
				auto newPlt = newPlottable_(i, oldPlt->name());

				// clone the data
				newPlt->setData(oldPlt->idata()); // 需要先设定data，cloneConfig才能正确工作

				newPlt->cloneConfig(*oldPlt);

				// 同步port2Plts_
				for (auto& i : port2Plts_)
					if (i.second == oldPlt) {
						i.second = newPlt;  break;
					}

				plot_->setPlottableAt(idx, newPlt);

				typeChanged = true;
			}
		}

		ImGui::EndCombo();
	}

	if (typeChanged)
		return true;

	if (data->isContinued()) {
		std::uint32_t minCount(1), maxCount(std::pow(1024, 1. / data->dim()));
		std::vector<std::uint32_t> c(data->dim());
		for (unsigned i = 0; i < data->dim(); i++)
		    c[i] = plt->sampCount(i);

		if (ImGui::SliderScalarN("Sampling Count", ImGuiDataType_U32,
			c.data(), data->dim(), &minCount, &maxCount)) {
			for (unsigned i = 0; i < data->dim(); i++)
			    plt->setSampCount(i, c[i]);
		}
	}

	return false;
}


void KvRdPlot::showPlottableArrangeProperty_(KvPlottable* plt)
{
	static const char* arrangeModes[] = {
		"overlay",
		"group",
		"ridge",
		"facet",
		"stack"
	};

	auto plt1d = dynamic_cast<KvPlottable1d*>(plt);
	if (!plt1d) return;

	auto disc = std::dynamic_pointer_cast<const KvDiscreted>(plt->odata());
	if (!disc || disc->isSampled()) {
		if (ImGuiX::treePush("Arrange", false)) {
			std::string label("Dim1");
			for (unsigned d = 0; d < plt->odata()->dim(); d++) {

				if (d == plt->odata()->dim() - 1) {
					if (plt->odata()->channels() == 1)
						break;
					label = "Channel";
				}
				else if (disc && disc->size(d) < 2) { 
					continue;
				}

				if (ImGuiX::treePush(label.c_str(), false)) {
					int mode = plt1d->arrangeMode(d);
					if (ImGui::Combo("Mode", &mode, arrangeModes, std::size(arrangeModes)))
						plt1d->setArrangeMode(d, mode);

					auto axis = plt1d->deltaAxis(d, true);
					if (axis != -1) {
						assert(axis <= plt1d->odim() && !plt1d->empty());
						float offset = plt1d->offset(d);
						float v_speed = plt1d->odata()->range(axis).length() / 100;
						if (ImGui::DragFloat("Offset", &offset, v_speed / 10))
							plt1d->setOffset(d, offset);

						float shift = plt1d->shift(d);
						if (ImGui::DragFloat("Shift", &shift, v_speed))
							plt1d->setShift(d, shift);
					}

					ImGuiX::treePop();
				}

				label.back()++; // "Dim1" -> "Dim2" -> "Dim3" -> ...
			}

			ImGuiX::treePop();
		}
	}
}


void KvRdPlot::showPlottableColoringProperty_(KvPlottable* plt)
{
	static const char* modeNames[] = {
		"one-color solid",
		"one-color gradiant",
		"two-color gradiant",
		"colorbar gradiant",
	};

	if (ImGuiX::treePush("Coloring", false)) {

		int mode = plt->coloringMode();
		if (ImGui::Combo("Mode", &mode, modeNames, std::size(modeNames)))
			plt->setColoringMode(KvPlottable::KeColoringMode(mode));

		// 主色
		if (mode == KvPlottable::k_colorbar_gradiant) { // 连续色，使用gradient
			static float selectedKey;
			auto grad = plt->gradient();
			if (ImGuiX::gradient("Colorbar", grad, selectedKey))
				plt->setGradient(grad);
		}
		else {
			if (plt->majorColors() == 1) { // 单主色
				auto clr = plt->majorColor(0);
				if (ImGui::ColorEdit4("Major Color", clr))
					plt->setMajorColors({ clr });
			}
			else { // 多主色
				std::vector<color4f> majors(plt->majorColors());
				for (unsigned i = 0; i < plt->majorColors(); i++)
					majors[i] = plt->majorColor(i);
				if (ImGuiX::multiColorsEdit("Major Colors", (ImVec4*)majors.data(), majors.size(), true))
					plt->setMajorColors(majors);
			}
		}

		// 辅色
		auto clr = plt->minorColor();
		if (ImGui::ColorEdit4("Minor Color", clr))
			plt->setMinorColor(clr);

		// 渐变映射有关属性
		if (mode != KvPlottable::k_one_color_solid) {

			bool b = plt->flatShading();
			if (ImGui::Checkbox("Flat Shading", &b))
				plt->setFlatShading(b);

			if (mode == KvPlottable::k_one_color_gradiant) {
				auto coeff = plt->brightenCoeff();
				if (ImGui::SliderFloat("Brighten Coeff", &coeff, -1, 1, "%.2f"))
					plt->setBrightenCoeff(coeff);
			}

			if (!plt->empty()) {
				int dim = plt->colorMappingDim() + 1;
				if (ImGui::SliderInt("Coloring Dim", &dim, 1, plt->odata()->dim() + 1))
					plt->setColorMappingDim(dim - 1);

				ImGui::Checkbox("Auto Range", &plt->autoColorMappingRange());

				auto r = plt->colorMappingRange();
				float low = r.first, high = r.second;
				if (ImGui::DragFloatRange2("Range", &low, &high)) {
					r.first = low, r.second = high;
					plt->setColorMappingRange(r);
					plt->autoColorMappingRange() = false;
				}
			}
		}

		ImGuiX::treePop();
	}
}


void KvRdPlot::showPlottableDefaultZProperty_(KvPlottable* plt)
{
	if (ImGuiX::treePush("Z Value", false)) {

		auto val = plt->defaultZ();
		if (ImGui::DragScalar("Default", ImGuiDataType_Double, &val))
			plt->setDefaultZ(val);

		val = plt->stepZ();
		if (ImGui::DragScalar("Step", ImGuiDataType_Double, &val))
			plt->setStepZ(val);

		auto b = plt->forceDefaultZ();
		if (ImGui::Checkbox("Force Default", &b))
			plt->setForceDefaultZ(b);

		ImGuiX::treePop();
	}
}


void KvRdPlot::showPlottableSplitAxesProperty_(KvPlottable* plt)
{
	if (plot_->dim() > 2)
		return; // plot3d不支持分离坐标轴

	if (ImGuiX::treePush("Split Axes", false)) {

		static const char* xmodeStr[] = { "share main", "split at bottom", "split at top" };
		static const char* ymodeStr[] = { "share main", "split at left", "split at right" };

		int xmode = plt->axis(0)->main() ? 0 : plt->axis(0)->type() == KcAxis::k_bottom ? 1 : 2;
		int ymode = plt->axis(1)->main() ? 0 : plt->axis(1)->type() == KcAxis::k_left ? 1 : 2;

		auto& coord2d = (KcCoord2d&)plot_->coord();
		
		bool open = ImGuiX::treePush("##SPLITX", false);
		ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::Combo("X", &xmode, coord2d.axisSwapped() ? ymodeStr : xmodeStr, std::size(xmodeStr))) 
			coord2d.splitAxis(plt, 0, xmode);
		if (open) {
			if (showAxisProperty_(*plt->axis(0)))
				plt->setDataChanged(false);
			ImGuiX::treePop();
		}

		open = ImGuiX::treePush("##SPLITY", false);
		ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::Combo("Y", &ymode, coord2d.axisSwapped() ? xmodeStr : ymodeStr, std::size(ymodeStr))) 
			coord2d.splitAxis(plt, 1, ymode);
		if (open) {
			if (showAxisProperty_(*plt->axis(1)))
				plt->setDataChanged(false);
			ImGuiX::treePop();
		}

		ImGuiX::treePop();
	}
}


void KvRdPlot::onThemeChanged(int type)
{
	auto& themeMgr = KsThemeManager::singleton();
	KcThemedPlotImpl_ tp(*plot_);

	switch (type)
	{
	case KsThemeManager::k_theme:
		themeMgr.applyTheme(curTheme_[type].first, curTheme_[type].second, &tp);

		// 同步包含的canvas, layout, palette等主题选项
		curTheme_[KsThemeManager::k_canvas].second = themeMgr.canvasName(curTheme_[type].first, curTheme_[type].second);
		curTheme_[KsThemeManager::k_layout].second = themeMgr.layoutName(curTheme_[type].first, curTheme_[type].second);
		curTheme_[KsThemeManager::k_palette].second = themeMgr.paletteName(curTheme_[type].first, curTheme_[type].second);

		// TODO: adjust the group name
		curTheme_[KsThemeManager::k_canvas].first = curTheme_[KsThemeManager::k_layout].first =
			curTheme_[KsThemeManager::k_palette].first = curTheme_[type].first;
		break;

	case KsThemeManager::k_layout:
		themeMgr.applyLayout(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	case KsThemeManager::k_canvas:
		themeMgr.applyCanvas(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	case KsThemeManager::k_palette:
		themeMgr.applyPalette(curTheme_[type].first, curTheme_[type].second, &tp);
		break;

	default:
		assert(false);
		break;
	}
}


void KvRdPlot::onDoubleClicked()
{
	if (plot_)
		plot_->setVisible(true);
}


void KvRdPlot::updateTheme_()
{
	if (!curTheme_[KsThemeManager::k_theme].second.empty()) {
		KcThemedPlotImpl_ tp(*plot_);

		// TODO: 这种方式不能完全复现
		KsThemeManager::singleton().applyTheme(
			curTheme_[KsThemeManager::k_theme].first, curTheme_[KsThemeManager::k_theme].second, &tp);
	}
}


namespace kPrivate
{
	static bool streamable(const KvSampled& samp1, const KvSampled& samp2)
	{
		if (samp1.dim() != samp2.dim() || samp1.channels() != samp2.channels())
			return false;

		for (unsigned i = 0; i < samp1.dim(); i++)
			if (samp1.step(i) != samp2.step(i))
				return false;

		for (unsigned d = 1; d < samp1.dim(); d++)
			if (samp1.size(d) != samp2.size(d)) // 只允许0维度（滑动维度）的尺寸不一致
				return false;

		return true;
	}

	template<int DIM>
	static std::shared_ptr<KvData> streaming_(std::shared_ptr<KvData> curData, std::shared_ptr<KvData> newData, unsigned nx)
	{
		auto samp = std::dynamic_pointer_cast<KtSampledArray<DIM>>(curData);

		if (nx == 0) {
			samp->clear();
			return samp;
		}

		/// 检测参数规格的一致性，不一致则直接复制newData
		auto newSamp = std::dynamic_pointer_cast<KvSampled>(newData);		
		if (curData == nullptr || !streamable(*samp, *newSamp)) {
			// NB: 此处不能直接返回newSamp，否则会破坏输入节点的数据缓存
			return std::make_shared<KtSampledArray<DIM>>(*newSamp);
		}

		/// 滑动数据
		auto sampArray = std::dynamic_pointer_cast<KtSampledArray<DIM>>(newData);
		if (sampArray) { // 使用快速版本
			samp->shift(*sampArray, nx);
		}
		else { // 使用通用版本
			samp->shift(*std::dynamic_pointer_cast<KvSampled>(newData), nx);
		}

		return samp;
	}
}

KvRdPlot::data_ptr KvRdPlot::streaming_(data_ptr curData, data_ptr newData, unsigned nx)
{
	// NB: curData可能为null（当数据源由static变为stream时）

	if (newData == nullptr)
		return curData;

	if (newData->dim() == 1)
		return kPrivate::streaming_<1>(curData, newData, nx);
	else if (newData->dim() == 2)
		return kPrivate::streaming_<2>(curData, newData, nx);
	else {
		assert(false);
	}

	return newData; // no streaming
}