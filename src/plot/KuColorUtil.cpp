#include "KuColorUtil.h"
#include "KuStrUtil.h"
#include "KuLex.h"


namespace kPrivate
{
	static int hexValue(char high, char low)
	{
		auto h = KuLex::hexValue(high);
		auto l = KuLex::hexValue(low);
		if (h == -1 || l == -1)
			return -1;

		return h * 16 + l;
	}

	// 返回的值为规范化的值，即值域在[0, 1]区间
	// @maxValue: 用来作规范化的数值
	// @defaultValue: 当str解析失败时返回的值
	static float parseColorComponent(const std::string_view& str, float maxValue = 1, float defaultValue = 0) 
	{
		auto val = KuStrUtil::toFloat<false>(str);
		if (!val) 
			return defaultValue;

		auto f = val.value();

		if (str.back() == '%') 
			f = KuMath::clamp(f, 0.f, 100.f) / 100;
		else 
			f /= maxValue;

		return f;
	}

	static bool validColorComponent(float val) 
	{
		return val >= 0 && val <= 1;
	}

	static float hue(float p, float q, float t) 
	{
		if (t < 0) { t += 1; }
		if (t > 1) { t -= 1; }
		if (t < 1.f / 6.f) { return p + (q - p) * 6.f * t; }
		if (t < 1.f / 2.f) { return q; }
		if (t < 2.f / 3.f) { return p + (q - p) * (2.f / 3.f - t) * 6.f; }
		return p;
	};
}


color4f KuColorUtil::parseColor(const std::string_view& str)
{
	static const std::string_view names[][2] = {
		{ "aliceblue", "#f0f8ff" },
		{ "antiquewhite", "#faebd7" },
		{ "aqua", "#0ff" },
		{ "aquamarine", "#7fffd4"},
		{ "azure", "#f0ffff" },
		{ "beige", "#f5f5dc" },
		{ "bisque", "#ffe4c4" },
		{ "black", "#000" },
		{ "blanchedalmond", "#ffebcd" },
		{ "blue", "#00f" },
		{ "blueviolet", "#8a2be2" },
		{ "brown", "#a52a2a" },
		{ "burlywood", "#deb887" },
		{ "cadetblue", "#5f9ea0" },
		{ "chartreuse", "#7fff00" },
		{ "chocolate", "#d2691e" },
		{ "coral", "#ff7f50" },
		{ "cornflowerblue", "#6495ed" },
		{ "cornsilk", "#fff8dc" },
		{ "crimson", "#dc143c" },
		{ "cyan", "#0ff" },
		{ "darkblue", "#00008b" },
		{ "darkcyan", "#008b8b" },
		{ "darkgoldenrod", "#b8860b" },
		{ "darkgray", "#a9a9a9" },
		{ "darkgreen", "#006400" },
		{ "darkgrey", "#a9a9a9" },
		{ "darkkhaki", "#bdb76b" },
		{ "darkmagenta", "#8b008b" },
		{ "darkolivegreen", "#556b2f" },
		{ "darkorange", "#ff8c00" },
		{ "darkorchid", "#9932cc" },
		{ "darkred", "#8b0000" },
		{ "darksalmon", "#e9967a" },
		{ "darkseagreen", "#8fbc8f" },
		{ "darkslateblue", "#483d8b" },
		{ "darkslategray", "#2f4f4f" },
		{ "darkslategrey", "#2f4f4f" },
		{ "darkturquoise", "#00ced1" },
		{ "darkviolet", "#9400d3" },
		{ "deeppink", "#ff1493" },
		{ "deepskyblue", "#00bfff" },
		{ "dimgray", "#696969" },
		{ "dimgrey", "#696969" },
		{ "dodgerblue", "#1e90ff" },
		{ "firebrick", "#b22222" },
		{ "floralwhite", "#fffaf0" },
		{ "forestgreen", "#228b22" },
		{ "fuchsia", "#f0f" },
		{ "gainsboro", "#dcdcdc" },
		{ "ghostwhite", "#f8f8ff" },
		{ "gold", "#ffd700" },
		{ "goldenrod", "#daa520" },
		{ "gray", "#808080" },
		{ "green", "#008000" },
		{ "greenyellow", "#adff2f" },
		{ "grey", "#808080" },
		{ "honeydew", "#f0fff0" },
		{ "hotpink", "#ff69b4" },
		{ "indianred", "#cd5c5c" },
		{ "indigo", "#4b0082" },
		{ "ivory", "#fffff0" },
		{ "khaki", "#f0e68c" },
		{ "lavender", "#e6e6fa" },
		{ "lavenderblush", "#fff0f5" },
		{ "lawngreen", "#7cfc00" },
		{ "lemonchiffon", "#fffacd" },
		{ "lightblue", "#add8e6" },
		{ "lightcoral", "#f08080" },
		{ "lightcyan", "#e0ffff" },
		{ "lightgoldenrodyellow", "#fafad2" },
		{ "lightgray", "#d3d3d3" },
		{ "lightgreen", "#90ee90" },
		{ "lightgrey", "#d3d3d3" },
		{ "lightpink", "#ffb6c1" },
		{ "lightsalmon", "#ffa07a" },
		{ "lightseagreen", "#20b2aa" },
		{ "lightskyblue", "#87cefa" },
		{ "lightslategray", "#789" },
		{ "lightslategrey", "#789" },
		{ "lightsteelblue", "#b0c4de" },
		{ "lightyellow", "#ffffe0" },
		{ "lime", "#0f0" },
		{ "limegreen", "#32cd32" },
		{ "linen", "#faf0e6" },
		{ "magenta", "#f0f" },
		{ "maroon", "#800000" },
		{ "mediumaquamarine", "#66cdaa" },
		{ "mediumblue", "#0000cd" },
		{ "mediumorchid", "#ba55d3" },
		{ "mediumpurple", "#9370db" },
		{ "mediumseagreen", "#3cb371" },
		{ "mediumslateblue", "#7b68ee" },
		{ "mediumspringgreen", "#00fa9a" },
		{ "mediumturquoise", "#48d1cc" },
		{ "mediumvioletred", "#c71585" },
		{ "midnightblue", "#191970" },
		{ "mintcream", "#f5fffa" },
		{ "mistyrose", "#ffe4e1" },
		{ "moccasin", "#ffe4b5" },
		{ "navajowhite", "#ffdead" },
		{ "navy", "#000080" },
		{ "oldlace", "#fdf5e6" },
		{ "olive", "#808000" },
		{ "olivedrab", "#6b8e23" },
		{ "orange", "#ffa500" },
		{ "orangered", "#ff4500" },
		{ "orchid", "#da70d6" },
		{ "palegoldenrod", "#eee8aa" },
		{ "palegreen", "#98fb98" },
		{ "paleturquoise", "#afeeee" },
		{ "palevioletred", "#db7093" },
		{ "papayawhip", "#ffefd5" },
		{ "peachpuff", "#ffdab9" },
		{ "peru", "#cd853f" },
		{ "pink", "#ffc0cb" },
		{ "plum", "#dda0dd" },
		{ "powderblue", "#b0e0e6" },
		{ "purple", "#800080" },
		{ "rebeccapurple", "#639" },
		{ "red", "#f00" },
		{ "rosybrown", "#bc8f8f" },
		{ "royalblue", "#4169e1" },
		{ "saddlebrown", "#8b4513" },
		{ "salmon", "#fa8072" },
		{ "sandybrown", "#f4a460" },
		{ "seagreen", "#2e8b57" },
		{ "seashell", "#fff5ee" },
		{ "sienna", "#a0522d" },
		{ "silver", "#c0c0c0" },
		{ "skyblue", "#87ceeb" },
		{ "slateblue", "#6a5acd" },
		{ "slategray", "#708090" },
		{ "slategrey", "#708090" },
		{ "snow", "#fffafa" },
		{ "springgreen", "#00ff7f" },
		{ "steelblue", "#4682b4" },
		{ "tan", "#d2b48c" },
		{ "teal", "#008080" },
		{ "thistle", "#d8bfd8" },
		{ "tomato", "#ff6347" },
		{ "turquoise", "#40e0d0" },
		{ "violet", "#ee82ee" },
		{ "wheat", "#f5deb3" },
		{ "white", "#fff" },
		{ "whitesmoke", "#f5f5f5" },
		{ "yellow", "#ff0" },
		{ "yellowgreen", "#9acd32" },
		{ "transparent", "#00000000" }
	};

	color4f clr(-1);

	if (str[0] == '#') {
		switch (str.size())
		{
		case 4: // #rgb
			clr.r() = kPrivate::hexValue(str[1], str[1]) / 255.f;
			clr.g() = kPrivate::hexValue(str[2], str[2]) / 255.f;
			clr.b() = kPrivate::hexValue(str[3], str[3]) / 255.f;
			clr.a() = 1;
			break;

		case 7: // #rrggbb
			clr.r() = kPrivate::hexValue(str[1], str[2]) / 255.f;
			clr.g() = kPrivate::hexValue(str[3], str[4]) / 255.f;
			clr.b() = kPrivate::hexValue(str[5], str[6]) / 255.f;
			clr.a() = 1;
			break;
			
		case 9: // #rrggbbaa
			clr.r() = kPrivate::hexValue(str[1], str[2]) / 255.f;
			clr.g() = kPrivate::hexValue(str[3], str[4]) / 255.f;
			clr.b() = kPrivate::hexValue(str[5], str[6]) / 255.f;
			clr.a() = kPrivate::hexValue(str[7], str[8]) / 255.f;
			break;

		default:
			break;
		}
	}
	else if (KuStrUtil::beginWith(str.data(), "rgb(", true) && KuStrUtil::endWith(str.data(), ")")){
		auto s = str.substr(4);
		s.remove_suffix(1); // remove the tailing ')'
		auto rgb = KuStrUtil::split(s, ",");
		if (rgb.size() == 3) {
			clr.r() = kPrivate::parseColorComponent(rgb[0], 255);
			clr.g() = kPrivate::parseColorComponent(rgb[1], 255);
			clr.b() = kPrivate::parseColorComponent(rgb[2], 255);
			clr.a() = 1;
		}
	}
	else if (KuStrUtil::beginWith(str.data(), "rgba(", true) && KuStrUtil::endWith(str.data(), ")")) {
		auto s = str.substr(5);
		s.remove_suffix(1); // remove the tailing ')'
		auto rgb = KuStrUtil::split(s, ",");
		if (rgb.size() == 4) {
			clr.r() = kPrivate::parseColorComponent(rgb[0], 255);
			clr.g() = kPrivate::parseColorComponent(rgb[1], 255);
			clr.b() = kPrivate::parseColorComponent(rgb[2], 255);
			clr.a() = kPrivate::parseColorComponent(rgb[3]);
		}
	}
	else if (KuStrUtil::beginWith(str.data(), "hsl(", true) && KuStrUtil::endWith(str.data(), ")")) {
		auto s = str.substr(4);
		s.remove_suffix(1); // remove the tailing ')'
		auto hsl = KuStrUtil::split(s, ",");
		if (hsl.size() == 3) {
			auto h = kPrivate::parseColorComponent(hsl[0], 360);
			auto s = kPrivate::parseColorComponent(hsl[1]);
			auto l = kPrivate::parseColorComponent(hsl[2]);
			clr = hslToRgb(h, s, l);
		}
	}
	else if (KuStrUtil::beginWith(str.data(), "hsla(", true) && KuStrUtil::endWith(str.data(), ")")) {
		auto s = str.substr(5);
		s.remove_suffix(1); // remove the tailing ')'
		auto hsl = KuStrUtil::split(s, ",");
		if (hsl.size() == 3) {
			auto h = kPrivate::parseColorComponent(hsl[0], 360);
			auto s = kPrivate::parseColorComponent(hsl[1]);
			auto l = kPrivate::parseColorComponent(hsl[2]);
			auto a = kPrivate::parseColorComponent(hsl[3]);
			clr = hslToRgb(h, s, l, a);
		}
	}
	else { // search for named color
		for(unsigned i = 0; i < std::size(names); i++)
			if (str == names[i][0]) {
				clr = parseColor(names[i][1]);
				break;
			}
	}

	if (!kPrivate::validColorComponent(clr.r()) 
		|| !kPrivate::validColorComponent(clr.g()) 
		|| !kPrivate::validColorComponent(clr.b())
		|| !kPrivate::validColorComponent(clr.a()))
		clr = color4f::invalid();

	return clr;
}


color4f KuColorUtil::hslToRgb(float h, float s, float l, float a)
{
	color4f clr;
	if (s == 0) {
		clr.r() = clr.g() = clr.b() = l;
	}
	else {
		auto q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		auto p = 2 * l - q;
		clr.r() = kPrivate::hue(p, q, h + 1.f / 3.f);
		clr.g() = kPrivate::hue(p, q, h);
		clr.b() = kPrivate::hue(p, q, h - 1.f / 3.f);
	}

	clr.a() = a;
	return clr;
}
