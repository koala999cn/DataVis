#include "KuThemeParser.h"
#include "KuStrUtil.h"


bool KuThemeParser::isNull(const jvalue& jval)
{
	if (jval.is_null())
		return true;

	if (jval.is_string()) {
		auto s = jval.get<std::string>();
		return stricmp("NA", s.c_str()) == 0
			|| stricmp("N/A", s.c_str()) == 0
			|| stricmp("N.A.", s.c_str()) == 0;
	}

	return false;
}


void KuThemeParser::fill_value(const jvalue& jval, KpBrush& brush)
{
	if (isNull(jval)) {
		brush.style = KpBrush::k_solid;
	}
	else if (jval.is_string()) {
		color4f color;
		if (color_value(jval.get<std::string>(), color)) {
			brush.color = color;
			if (brush.style == KpBrush::k_none)
				brush.style = KpBrush::k_solid;
	    }
	}
	else if (jval.is_object()) {

	    // color
		if (jval.contains("color")) {
			color4f color;
			color_value(jval, color);
			if (color.isValid()) 
				brush.color = color;
		}
			
		// TODO: 
		// texture

		// gradient

		// style
	}
}


void KuThemeParser::line_value(const jvalue& jval, KpPen& pen)
{
	if (isNull(jval)) {
		pen.style = KpPen::k_none; 
		return;
	}
	
	if (jval.is_string()) {
		auto str = jval.get<std::string>();
		if (!line_style(str, pen))  // 先尝试解析line-style
			// 失败则尝试解析line-color
			if(line_color(str, pen) && pen.style == KpPen::k_none)
				pen.style = KpPen::k_solid;
	}
	else if (jval.is_number_float()) {
		auto w = jval.get<double>();
		pen.width = w;
	}
	else if (jval.is_object()) {

		// color
		color4f color;
		if (tryColor(jval, "color", color))
			pen.color = color;

		// width
		double width;
		if (tryDouble(jval, "width", width))
			pen.width = width;

		// style
		std::string style;
		if (tryString(jval, "style", style))
			line_style(style, pen);
	}
}


void KuThemeParser::text_value(const jvalue& jval, KpFont& font)
{
	if (isNull(jval) || jval.is_string()) {
		; // do nothing
	}
	else if (jval.is_number_float()) {
		auto w = jval.get<double>();
		font.size = w;
	}
	else if (jval.is_object()) {

		// family
		std::string family;
		if (tryString(jval, "family", family))
			font.family = family;

		// face
		std::set<std::string> face;
		if (tryStringEnum(jval, "face", face)) {
			font.bold = face.count("bold");
			font.italic = face.count("italic");
			font.underline = face.count("underline");
		}

		// size
		double size;
		if (tryDouble(jval, "size", size))
			font.size = size;
	}
}


void KuThemeParser::color_value(const jvalue& jval, color4f& color)
{
	if (isNull(jval))
		color = color4f("transparent");
	else if (jval.is_string())
		color_value(jval.get<std::string>(), color);
	else if (jval.is_object())
		tryColor(jval, "color", color);
}


std::vector<color4f> KuThemeParser::color_value_list(const jobject& jar)
{
	assert(jar.is_array());

	std::vector<color4f> colors;
	colors.reserve(jar.size());
	for (auto i = jar.cbegin(); i != jar.cend(); ++i) {
		color4f color;
		if (i->is_string() && color_value(i->get<std::string>(), color))
			colors.push_back(color);
	}

	return colors;
}


bool KuThemeParser::margins_value(const jvalue& jval, KtMargins<double>& margin)
{
	if (isNull(jval)) {
		margin = KtMargins<double>(0, 0, 0, 0);
	}
	else if (jval.is_array()) {
		std::vector<int> ints; ints.reserve(4);
		for (auto iter = jval.cbegin(); iter != jval.cend(); iter++) {
			if (iter->is_number_float())
				ints.push_back(iter->get<double>());
		}

		if(ints.empty())
			return false;

		// int left, int top, int right, int bottom
		margin.left() = ints[0];
		if (ints.size() > 1)
			margin.top() = ints[1];
		if (ints.size() > 2)
			margin.right() = ints[2];
		if (ints.size() > 3)
			margin.bottom() = ints[3];
	}
	else if (jval.is_object()) {
		int val;
		if (tryInt(jval, "left", val))
			margin.left() = val;
		if (tryInt(jval, "top", val))
			margin.top() = val;
		if (tryInt(jval, "right", val))
			margin.right() = val;
		if (tryInt(jval, "bottom", val))
			margin.bottom() = val;
	}
	else {
		return false;
	}

	return true; // TODO: 细化返回值
}


bool KuThemeParser::tryInt(const jobject& jobj, const std::string& name, int& val)
{
	if (jobj.contains(name) && jobj[name].is_number_integer()) {
		val = jobj[name].get<int>();
		return true;
	}

	return false;
}


bool KuThemeParser::tryDouble(const jobject& jobj, const std::string& name, double& val)
{
	if (jobj.contains(name) && jobj[name].is_number_float()) {
		val = jobj[name].get<double>();
		return true;
	}

	return false;
}


bool KuThemeParser::tryString(const jobject& jobj, const std::string& name, std::string& val)
{
	if (jobj.contains(name) && jobj[name].is_string()) {
		val = jobj[name].get<std::string>();
		return true;
	}

	return false;
}


bool KuThemeParser::tryStringEnum(const jobject& jobj, const std::string& name, std::set<std::string>& val)
{
	if (jobj.contains(name)) {
		if (jobj[name].is_string()) {
			val.insert(jobj[name].get<std::string>());
			return true;
		}
		else if (jobj[name].is_array()) {
			auto ar = jobj[name];
			for (auto iter = ar.begin(); iter != ar.end(); ++iter) {
				if (iter->is_string())
					val.insert(iter->get<std::string>());
			}
			return true;
		}
	}

	return false;
}


bool KuThemeParser::tryBool(const jobject& jobj, const std::string& name, bool& val)
{
	if (jobj.contains(name) && jobj[name].is_boolean()) {
		val = jobj[name].get<bool>();
		return true;
	}

	return false;
}


bool KuThemeParser::tryColor(const jobject& jobj, const std::string& name, color4f& color)
{
	std::string str;
	if (!tryString(jobj, name, str))
		return false;

	return color_value(str, color);
}


bool KuThemeParser::tryMargins(const jobject& jobj, const std::string& name, KtMargins<double>& margins)
{
	if (jobj.contains(name)) 
		return margins_value(jobj[name], margins);

	return false;
}


bool KuThemeParser::color_value(const std::string& str, color4f& color)
{
	
	// 扩展支持rgb()格式, 如: rgb(198, 120, 65)
	if (KuStrUtil::beginWith(str, "rgb(", true) && KuStrUtil::endWith(str, ")")) {
		
		auto s = KuStrUtil::mid(str, 4, -1);

		auto rgb = KuStrUtil::split(s, ",");
		if (rgb.size() != 3 || rgb.size() != 4)
			return false;

		color.r() = KuStrUtil::toFloat(rgb[0].c_str()) / 255;
		color.g() = KuStrUtil::toFloat(rgb[1].c_str()) / 255;
		color.b() = KuStrUtil::toFloat(rgb[2].c_str()) / 255;
		color.a() = rgb.size() == 4 ? KuStrUtil::toFloat(rgb[3].c_str()) / 255 : 1;	
	}
	else {
		// color4f支持的颜色字符格式：
		//  - #RGB(each of R, G, and B is a single hex digit)
		//	- #RRGGBB
		//	- #AARRGGBB(Since 5.2)
		//	- #RRRGGGBBB
		//	- #RRRRGGGGBBBB
		//	- transparent, representing the absence of a color
		//  - A list of SVG color keyword names provided by the World Wide Web Consortium.
		color.setNamedColor(str);
	}

	return color.isValid();
}


bool KuThemeParser::line_style(const std::string& str, KpPen& pen)
{
	if (str == "solid")
		pen.style = KpPen::k_solid;
	else if (str == "dash")
		pen.style = KpPen::k_dash;
	else if (str == "dot")
		pen.style = KpPen::k_dot;
	else if (str == "dashdot")
		pen.style = KpPen::k_dash_dot;
	else if (str == "dashdotdot")
		pen.style = KpPen::k_dash_dot_dot;
	else
		return false;

	return true;
}


bool KuThemeParser::line_color(const std::string& str, KpPen& pen)
{
	color4f color;
	if (!color_value(str, color))
		return false;

	pen.color = color;
	return true;
}

