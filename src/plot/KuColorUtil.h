#pragma once
#include <string_view>
#include "KtColor.h"


class KuColorUtil
{
public:

	/*
		parse a valid CSS color string into color4f
		valid input examples:
		red
		#f00
		#ff0000
		#ff0000ff
		rgb(255, 0, 0)
		rgb(100%, 0%, 0%)
		rgba(255, 0, 0, 1)
		rgba(100%, 0%, 0%, 1)
		hsl(0, 100%, 50%)
		hsla(0, 100%, 50%, 1)
	*/
	static color4f parseColor(const std::string_view& str);

	// Convert hsl to rgb, alpha value gets passed straight through
	// h, s, l values are assumed to be in interval [0, 1]
	// http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
	static color4f hslToRgb(float h, float s, float l, float a = 1);
};