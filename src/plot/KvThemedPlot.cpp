#include "KvThemedPlot.h"


int KvThemedPlot::enterLevel(int curLevel, int into)
{
	if (into & k_xy)
		curLevel &= ~k_xy;

	if (into & k_element)
		curLevel &= ~k_element;

	if (into & (k_text | k_line)) 
		curLevel &= ~(k_text | k_line);

	return curLevel | into;
}

