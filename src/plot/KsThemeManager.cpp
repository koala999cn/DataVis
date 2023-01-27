#include "KsThemeManager.h"
#include "KuPathUtil.h"
#include "KuThemeParser.h"
#include "KvThemedPlot.h"
#include "KtGradient.h"
#include <fstream>
#include <assert.h>


template<typename MAP>
std::vector<typename MAP::key_type> map_keys(const MAP& m)
{
	std::vector<typename MAP::key_type> keys;
	for (auto& i : m)
		keys.push_back(i.first);

	return keys;
}


bool KsThemeManager::load(const std::string_view& path)
{
	for (unsigned i = 0; i < 4; i++)
		global_[i].clear();
	grouped_.clear();

	auto files = KuPathUtil::getFiles(path.data(), false, true);
	for (auto fileName : files) {
		nlohmann::json;
		std::ifstream ifs(fileName);
		auto jdoc = nlohmann::json::parse(ifs);
		if (jdoc.empty()) continue;

		if (jdoc.is_object())
			tryLoad_(jdoc);
		else if (jdoc.is_array()) {
			for (auto i = jdoc.begin(); i != jdoc.end(); ++i)
				if (i->is_object()) // 不考虑递归数组的结构
					tryLoad_(*i);
		}
	}

	removeInvalidThemes_();

	return !empty();

}


bool KsThemeManager::empty() const
{
	return grouped_.empty() && isEmpty_(global_);
}


bool KsThemeManager::hasTheme() const
{
	return !isEmpty(k_theme);
}


bool KsThemeManager::hasLayout() const
{
	return !isEmpty(k_layout);
}


bool KsThemeManager::hasCanvas() const
{
	return !isEmpty(k_canvas);
}


bool KsThemeManager::hasPalette() const
{
	return !isEmpty(k_palette);
}


void KsThemeManager::tryLoad_(const jobject& jobj)
{
	if (jobj.contains("group") && jobj["group"].is_string()) {
		auto groupName = jobj["group"].get<std::string>();
		tryLoad_(grouped_[groupName], jobj);
		if (isEmpty_(grouped_[groupName]))
			grouped_.erase(groupName);
	}
	else {
		tryLoad_(global_, jobj);
	}
}


void KsThemeManager::tryLoad_(theme_type& theme, const jobject& jobj)
{
	static const char* themeName[] = {
		"theme", "layout", "canvas", "palette"
	};

	static const char* themeListName[] = {
		"theme-list", "layout-list", "canvas-list", "palette-list"
	};

	for (unsigned i = 0; i < 4; i++) {
		if (jobj.contains(themeName[i]))
			tryObjectOrArray_(jobj[themeName[i]], [&theme, i](const jobject& jobj) {
			if (jobj.contains("name") && jobj["name"].is_string()) // 忽略未命名的主题
				theme[i][jobj["name"].get<std::string>()] = jobj;
				});

		if (jobj.contains(themeListName[i]) && jobj[themeListName[i]].is_object())
			tryList_(jobj[themeListName[i]], [&theme, i](const std::string& key, const jvalue& jval) {
			if (jval.is_object())
				theme[i].emplace(key, jval);
				});
	}
}


void KsThemeManager::tryObjectOrArray_(const jvalue& jval, std::function<void(const jobject&)> fn)
{
	if (jval.is_object())
		fn(jval);
	else if (jval.is_array()) {
		for (auto i = jval.begin(); i != jval.end(); ++i)
			if (i->is_object())
				fn(*i);
	}
}


void KsThemeManager::tryList_(const jobject& jobj, std::function<void(const std::string& key, const jvalue&)> fn)
{
	for (auto i = jobj.begin(); i != jobj.end(); ++i)
	    fn(i.key(), *i);
}


void KsThemeManager::removeInvalidThemes_()
{
	// TODO:
}


bool KsThemeManager::isEmpty_(const theme_type& theme)
{
	for (unsigned i = 0; i < 4; i++)
		if (!theme[i].empty())
			return false;
	return true;
}


bool KsThemeManager::isEmpty(int themeType) const
{
	if (!global_[themeType].empty())
		return false;

	for (auto& i : grouped_)
		if (!i.second[themeType].empty())
			return false;

	return true;
}


KsThemeManager::stringlist KsThemeManager::listNames(int type, const std::string& groupName) const
{
	if (groupName.empty()) {
		return map_keys(global_[type]);
	}
	else {
		auto iter = grouped_.find(groupName);
		if (iter != grouped_.cend())
			return map_keys(iter->second[type]);
	}

	return {};
}


KsThemeManager::stringlist KsThemeManager::listGroups() const
{
	return map_keys(grouped_);
}


KsThemeManager::stringlist KsThemeManager::listThemes(const std::string& group) const
{
	return listNames(k_theme, group);
}


KsThemeManager::stringlist KsThemeManager::listCanvas(const std::string& group) const
{
	return listNames(k_canvas, group);
}


KsThemeManager::stringlist KsThemeManager::listPalettes(const std::string& group) const
{
	return listNames(k_palette, group);
}


KsThemeManager::stringlist KsThemeManager::listLayouts(const std::string& group) const
{
	return listNames(k_layout, group);
}


const KsThemeManager::jobject& KsThemeManager::themeAt_(int type, const std::string& group, const std::string& name) const
{
	if (group.empty())
		return global_[type].at(name);
	else
		return grouped_.at(group)[type].at(name);
}


std::string KsThemeManager::canvasName(const std::string& group, const std::string& theme)
{
	auto& jobj = themeAt_(k_theme, group, theme);
	if (jobj.contains("canvas") && jobj["canvas"].is_string())
		return jobj["canvas"].get<std::string>();

	return "";
}


std::string KsThemeManager::layoutName(const std::string& group, const std::string& theme)
{
	auto& jobj = themeAt_(k_theme, group, theme);
	if (jobj.contains("layout") && jobj["layout"].is_string())
		return jobj["layout"].get<std::string>();

	return "";
}


std::string KsThemeManager::paletteName(const std::string& group, const std::string& theme)
{
	auto& jobj = themeAt_(k_theme, group, theme);
	if (jobj.contains("palette") && jobj["palette"].is_string())
		return jobj["palette"].get<std::string>();

	return "";
}


void KsThemeManager::applyTheme(const std::string& group, const std::string& name, KvThemedPlot* plot) const
{
	auto& jobj = themeAt_(k_theme, group, name);

	if (jobj.contains("base") && jobj["base"].is_string())
		applyTheme(group, jobj["base"].get<std::string>(), plot);

	tryCanvas_(group, jobj, plot);
	tryPalette_(group, jobj, plot);
	tryLayout_(group, jobj, plot);
	applyLayout_(group, jobj, plot, true); // open 'inTheme' flag to disable "base" property
}


void KsThemeManager::applyLayout(const std::string& group, const std::string& name, KvThemedPlot* plot) const
{
	applyLayout_(group, themeAt_(k_layout, group, name), plot, false);
}


void KsThemeManager::applyCanvas(const std::string& group, const std::string& name, KvThemedPlot* plot) const
{
	applyCanvas_(themeAt_(k_canvas, group, name), plot);
}


void KsThemeManager::applyPalette(const std::string& group, const std::string& name, KvThemedPlot* plot) const
{
	applyPalette_(themeAt_(k_palette, group, name), plot);
}


void KsThemeManager::tryCanvas_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("canvas")) {
		auto canvas = jobj["canvas"];
		if (canvas.is_string())
			applyCanvas(group, canvas.get<std::string>(), plot);
		else 
			applyCanvas_(canvas, plot);
	}
}


void KsThemeManager::tryPalette_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("palette")) {
		auto palette = jobj["palette"];
		if (palette.is_string())
			applyPalette(group, palette.get<std::string>(), plot);
		else 
			applyPalette_(palette, plot);
	}
}


void KsThemeManager::tryLayout_(const std::string& group, const jobject& jobj, KvThemedPlot* plot) const
{
	if (jobj.contains("layout")) {
		auto layout = jobj["layout"];
		if (layout.is_string())
			applyLayout(group, layout.get<std::string>(), plot);
		else if (layout.is_object())
			applyLayout_(group, layout, plot, false);
	}
}


void KsThemeManager::applyLayout_(const std::string& group, const jobject& jobj, KvThemedPlot* plot, bool inTheme) const
{
	if (!inTheme) { // try base
		if (jobj.contains("base") && jobj["base"].is_string())
			applyLayout(group, jobj["base"].get<std::string>(), plot);
	}

	tryLevel_(KvThemedPlot::k_all, jobj, plot);
	tryBkgnd_(KvThemedPlot::k_plot, jobj, plot);
	tryAxis_(jobj, plot);
	tryGrid_(jobj, plot);
	tryMargins_(KvThemedPlot::k_plot, jobj, plot);
	tryLegend_(jobj, plot);
}


KsThemeManager::colorlist KsThemeManager::parseCanvas_(const jvalue& jval)
{
	colorlist cl(5, color4f(0));

	if (jval.is_object()) {
		auto& jobj = jval;
		static const char* names[] = {
			"background", "axis-rect", "text", "line", "gridline"
		};

		color4f clr;
		for(unsigned i = 0; i < std::size(names); i++)
			if (KuThemeParser::tryColor(jobj, names[i], clr))
				cl[i] = clr;
	}
	else if (jval.is_array()) {
		color4f clr;
		for (unsigned i = 0; i < jval.size(); i++) {
			if (jval.at(i).is_string() &&
				KuThemeParser::color_value(jval.at(i).get<std::string>(), clr))
				cl[i] = clr;
		}
	}

	return cl;
}


KsThemeManager::colorlist KsThemeManager::getCanvas(const std::string& group, const std::string& name)
{
	return parseCanvas_(themeAt_(k_canvas, group, name));
}


void KsThemeManager::applyCanvas_(const jvalue& jval, KvThemedPlot* plot) const
{
	if (jval.is_object()) {
		auto jobj = jval;

		tryBkgnd_(KvThemedPlot::k_plot, jobj, plot);

		if (jobj.contains("axis-rect"))
			applyFill_(KvThemedPlot::k_axis, jobj["axis-rect"], plot);

		if (jobj.contains("text"))
			applyTextColor_(KvThemedPlot::k_all_text, jobj["text"], plot);

		if (jobj.contains("line"))
			applyLine_(KvThemedPlot::k_all_line, jobj["line"], plot);

		if (jobj.contains("gridline"))
			applyLine_(KvThemedPlot::k_all_grid_line, jobj["gridline"], plot);
	}
	else if (jval.is_array()) {
		auto sz = jval.size();

		if (sz > 0)
			applyFill_(KvThemedPlot::k_plot, jval.at(0), plot);

		if (sz > 1)
			applyFill_(KvThemedPlot::k_axis, jval.at(1), plot);

		if (sz > 2)
			applyText_(KvThemedPlot::k_all_text, jval.at(2), plot);

		if (sz > 3)
			applyLine_(KvThemedPlot::k_all_line, jval.at(3), plot);

		if (sz > 4)
			applyLine_(KvThemedPlot::k_all_grid_line, jval.at(4), plot);
	}
}


void KsThemeManager::parsePalette_(const jvalue& jval, colorlist& majors, colorlist& minors)
{
	if (jval.is_array()) {
		majors = KuThemeParser::color_value_list(jval);
	}
	else if (jval.is_object()) {
		auto jobj = jval;
		if (jobj.contains("major") && jobj["major"].is_array())
			majors = KuThemeParser::color_value_list(jobj["major"]);
		if (jobj.contains("minor")) {
			if (jobj["minor"].is_array())
				minors = KuThemeParser::color_value_list(jobj["minor"]); // TODO: 调整minor只支持单色
			else if (jobj["minor"].is_string()) {
				color4f color;
				if (KuThemeParser::color_value(jobj["minor"].get<std::string>(), color))
					minors.push_back(color);
			}
		}
	}
}


bool KsThemeManager::getPalette(const std::string& group, const std::string& name, colorlist& majors, colorlist& minors)
{
	parsePalette_(themeAt_(k_palette, group, name), majors, minors);
	return true;
}


void KsThemeManager::applyPalette_(const jvalue& jval, KvThemedPlot* plot) const
{
	std::vector<color4f> majors;
	std::vector<color4f> minors; // 用于outlining

	parsePalette_(jval, majors, minors);

	if (majors.empty())
		return;

	if (minors.empty())
		minors.push_back(color4f(0, 0, 0, 1));

	// 统计该plot需要多少个主色调
	unsigned majorsNeeded(0);
	for (unsigned i = 0; i < plot->plottableCount(); i++) {
		auto n = plot->majorColorsNeeded(i);
		if (n != -1) // 只统计离散色
			majorsNeeded += n;
	}

	// 分配颜色
	unsigned idx(0);
	KtGradient<float, color4f> grad;

	if (majorsNeeded > majors.size()) 
		for (unsigned i = 0; i < majors.size(); i++)
			grad.setAt(i, majors[i]);

	for (unsigned i = 0; i < plot->plottableCount(); i++) {
		auto n = plot->majorColorsNeeded(i);
		if (n == -1) // 对于连续色，将全部主色都分配给它
			plot->applyMajorColors(i, majors);
		else {
			std::vector<color4f> clrs(n);
			if (majorsNeeded > majors.size()) { // 调色板颜色不足，使用gradient插值取色
				for (unsigned j = 0; j < n; j++)
					clrs[j] = grad.getAt(KuMath::remap<float>(idx++, 0, majorsNeeded - 1, 0, majors.size() - 1));
			}
			else { // 调色板颜色足够，按顺序分配
				for (unsigned j = 0; j < n; j++)
					clrs[j] = majors[idx++];
			}
			plot->applyMajorColors(i, clrs);
		}

		plot->applyMinorColor(i, minors.front());
	}
}


void KsThemeManager::tryBkgnd_(int level, const jobject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("background"))
		applyFill_(level, jobj["background"], plot);
}


void KsThemeManager::applyFill_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	KpBrush brush = plot->fill(level);
	KuThemeParser::fill_value(jval, brush);
	plot->applyFill(level, brush);
}


void KsThemeManager::tryVisible_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	if (KuThemeParser::isNull(jval)) {
		plot->applyVisible(level, false);
	}
	else if (jval.is_boolean()) {
		bool b = jval.get<bool>();
		plot->applyVisible(level, b);
	}
	else if (jval.is_object()) {
		bool b(true);
		if (KuThemeParser::tryBool(jval, "visible", b)) {
			plot->applyVisible(level, b);
		}
	}
}


void KsThemeManager::tryLevel_(int level, const jobject& jobj, KvThemedPlot* plot)
{
	trySpecialProp_(level, jobj, "line", KvThemedPlot::k_line,
		[plot](int newLevel, const jvalue& jval) {
			applyLine_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "title", KvThemedPlot::k_title,
		[plot](int newLevel, const jvalue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "label", KvThemedPlot::k_label,
		[plot](int newLevel, const jvalue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "text", KvThemedPlot::k_text,
		[plot](int newLevel, const jvalue& jval) {
			applyText_(newLevel, jval, plot);
			applyTextColor_(newLevel, jval, plot);
		});
}


void KsThemeManager::tryAxis_(const jobject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("axis")) return;
	
	auto axis = jobj["axis"];

	// 单独解析axis对象的背景属性，axis的其他元素没有该属性
	if (axis.is_object()) {
		auto jobj = axis;
		tryBkgnd_(KvThemedPlot::k_axis, jobj, plot);
		tryMargins_(KvThemedPlot::k_axis, jobj, plot);
	}

	// 解析每个方向(left, top, right, bottom)坐标轴的属性
	applyAxis_(KvThemedPlot::k_all_axis_all, axis, plot);
}


void KsThemeManager::applyAxis_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	if (!jval.is_object()) 
		return;

	applyLine_(level, jval, plot); // 放在is_object检测之后，主要为防止applyLine_再次进行tryVisible_检测

	auto jobj = jval;

	/// 首先处理公用属性
	tryLevel_(level, jobj, plot);

	trySpecialProp_(level, jobj, "baseline", KvThemedPlot::k_baseline,
		[plot](int newLevel, const jvalue& jval) {
			applyLine_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "tick", KvThemedPlot::k_axis_tick,
		[plot](int newLevel, const jvalue& jval) {
			applyTick_(newLevel, jval, plot);
		});


	/// 然后处理特化属性
	tryAxisSpecials_(level, jobj, plot, applyAxis_);
}


void KsThemeManager::applyTick_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	assert(level & KvThemedPlot::k_axis_tick);
	assert(!(level & KvThemedPlot::k_text));

	tryVisible_(level, jval, plot);

	if (!jval.is_object())
		return;

	applyLine_(level, jval, plot); // 放在is_object检测之后，主要为防止applyLine_再次进行tryVisible_检测

	auto jobj = jval;

	std::string str;
	KvThemedPlot::KeTickSide side = KvThemedPlot::k_tick_none;
	if (KuThemeParser::tryString(jobj, "side", str)) {
		if (str == "in")
			side = KvThemedPlot::k_tick_inside;
		else if (str == "out")
			side = KvThemedPlot::k_tick_outside;
		else if (str == "both")
			side = KvThemedPlot::k_tick_bothside;
	}

	int len(-1);
	if(KuThemeParser::tryInt(jobj, "length", len))
		plot->setTickLength(level, side, len);


	trySpecialProp_(level, jobj, "major", KvThemedPlot::k_majorline, 
		[plot](int newLevel, const jvalue& jval) {
			applyTick_(newLevel, jval, plot);
		});

	trySpecialProp_(level, jobj, "minor", KvThemedPlot::k_minorline,
		[plot](int newLevel, const jvalue& jval) {
			applyTick_(newLevel, jval, plot);
		});

	/// 处理特化属性
	tryAxisSpecials_(level, jobj, plot, applyTick_);
}


void KsThemeManager::tryGrid_(const jobject& jobj, KvThemedPlot* plot)
{
	if (!jobj.contains("grid")) return;

	auto grid = jobj["grid"];

	applyGrid_(KvThemedPlot::k_all_grid_all, grid, plot);
}


void KsThemeManager::applyGrid_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	assert(level & KvThemedPlot::k_grid);

	tryVisible_(level, jval, plot); 

	if (!jval.is_object())
		return; 

	applyLine_(level, jval, plot); // 放在is_object检测之后，主要为防止applyLine_再次进行tryVisible_检测

	auto jobj = jval;

	/// 1. 处理公用属性(line, text, label, ...)
	tryLevel_(level, jobj, plot);

	/// 2. 处理特有属性
	static const std::pair<const char*, int> prop[] = {
		{ "major", KvThemedPlot::k_grid_major },
		{ "minor", KvThemedPlot::k_grid_minor },
		{ "zeroline", KvThemedPlot::k_grid_zeroline }
	};
	for (unsigned i = 0; i < std::size(prop); i++) 
		trySpecialProp_(level, jobj, prop[i].first, prop[i].second,
			[plot](int newLevel, const jvalue& jval) {
				assert(newLevel & KvThemedPlot::k_grid);
				applyLine_(newLevel, jval, plot);
			}
	    );


	/// 3. 处理特化属性(x, y, left, ...)
	tryGridSpecials_(level, jobj, plot, applyGrid_);
}


// TODO: 按照level处理
void KsThemeManager::tryMargins_(int level, const jobject& jobj, KvThemedPlot* plot)
{
	if (jobj.contains("margins")) {
		auto margins = plot->margins(level);
		if (KuThemeParser::margins_value(jobj["margins"], margins))
			plot->applyMargins(level, margins);
	}
}


void KsThemeManager::tryLegend_(const jobject& jobj, KvThemedPlot* plot)
{

}


void KsThemeManager::tryAxisSpecials_(int level, const jobject& jobj, KvThemedPlot* plot,
	std::function<void(int, const jvalue&, KvThemedPlot*)> op)
{
	static const std::pair<const char*, int> special[] = {
		{ "left", KvThemedPlot::k_left },
		{ "top", KvThemedPlot::k_top },
		{ "right", KvThemedPlot::k_right },
		{ "bottom", KvThemedPlot::k_bottom },
		{ "near-left", KvThemedPlot::k_near_left },
		{ "near-top", KvThemedPlot::k_near_top },
		{ "near-right", KvThemedPlot::k_near_right },
		{ "near-bottom", KvThemedPlot::k_near_bottom },
		{ "far-left", KvThemedPlot::k_far_left },
		{ "far-top", KvThemedPlot::k_far_top },
		{ "far-right", KvThemedPlot::k_far_right },
		{ "far-bottom", KvThemedPlot::k_far_bottom },
		{ "floor-left", KvThemedPlot::k_floor_left },
		{ "floor-right", KvThemedPlot::k_floor_right },
		{ "ceil-left", KvThemedPlot::k_ceil_left },
		{ "ceil-right", KvThemedPlot::k_ceil_right }
	};

	for (unsigned i = 0; i < std::size(special); i++)
		trySpecialProp_(level, jobj, special[i].first, special[i].second, 
			[plot, op](int newLevel, const jvalue& jval) {
				op(newLevel, jval, plot);
			}
		);
}


void KsThemeManager::tryGridSpecials_(int level, const jobject& jobj, KvThemedPlot* plot,
	std::function<void(int, const jvalue&, KvThemedPlot*)> op)
{
	static const std::pair<const char*, int> special[] = {
		{ "front", KvThemedPlot::k_plane_front },
		{ "back", KvThemedPlot::k_plane_back },
		{ "left", KvThemedPlot::k_plane_left },
		{ "right", KvThemedPlot::k_plane_right },
		{ "floor", KvThemedPlot::k_plane_floor },
		{ "ceil", KvThemedPlot::k_plane_ceil }
	};

	for (unsigned i = 0; i < std::size(special); i++)
		trySpecialProp_(level, jobj, special[i].first, special[i].second,
			[plot, op](int newLevel, const jvalue& jval) {
				op(newLevel, jval, plot);
			}
	);
}


void KsThemeManager::trySpecialProp_(int level, const jobject& jobj, const char* propName, int propId,
	std::function<void(int, const jvalue&)> op)
{
	auto newLevel = KvThemedPlot::enterLevel(level, propId);
	if (newLevel && jobj.contains(propName))
		op(newLevel, jobj[propName]);
}


void KsThemeManager::applyLine_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	plot->applyLine(level, [&jval, level](const KpPen& pen) {
		KpPen newPen(pen);
		KuThemeParser::line_value(jval, newPen);
		return newPen;
		});
}


void KsThemeManager::applyText_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	tryVisible_(level, jval, plot);

	plot->applyText(level, [&jval](const KpFont& font) {
		KpFont newFont(font);
		KuThemeParser::text_value(jval, newFont);
		return newFont;
		});
}


void KsThemeManager::applyTextColor_(int level, const jvalue& jval, KvThemedPlot* plot)
{
	plot->applyTextColor(level, [&jval](const color4f& color) {
		color4f newColor(color);
		KuThemeParser::color_value(jval, newColor);
		return newColor;
		});
}
