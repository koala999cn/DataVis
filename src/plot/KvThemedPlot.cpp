#include "KvThemedPlot.h"


int KvThemedPlot::enterLevel(int curLevel, int into)
{
	if (into & k_all_axis)
		curLevel &= ~k_all_axis;

	if (into & k_all_plane)
		curLevel &= ~k_all_plane;

	if (into & k_element)
		curLevel &= ~k_element;

	if (into & (k_text | k_line)) 
		curLevel &= ~(k_text | k_line);

	return curLevel | into;
}

