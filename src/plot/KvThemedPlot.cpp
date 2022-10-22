#include "KvThemedPlot.h"


int KvThemedPlot::enterLevel(int curLevel, int into)
{
	if (into & k_element)
		curLevel &= ~k_element;

	if (into & k_all_axis_direction)
		curLevel &= ~k_all_axis_direction;

	if (into & k_all_plane_direction)
		curLevel &= ~k_all_plane_direction;

	if (into & (k_text | k_line)) 
		curLevel &= ~(k_text | k_line);

	return curLevel | into;
}

