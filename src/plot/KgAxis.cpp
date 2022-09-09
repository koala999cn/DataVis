#include "KgAxis.h"
#include <assert.h>


KgAxis::KgAxis() 
{
	lower_ = upper_ = 0;

	visible_ = true;
	showAll();

	baselineSize_ = 1;
	tickSize_ = 1, tickLength_ = 1;
	subtickSize_ = 1, subtickLength_ = 1;

	baselineColor_ = vec4(0, 0, 0, 1);
	tickColor_ = subtickColor_ = vec4(0, 0, 0, 1);
	labelColor_ = titleColor_ = vec4(0, 0, 0, 1);

	//labelFont_, titleFont_; // TODO:

	tickShowBothSide_ = false;
}


void KgAxis::setTickOrient(KeTickOrient to)
{
	switch (to)
	{
	case k_x:
		setTickOrient(vec3(1, 0, 0), false);
		break;

	case k_neg_x:
		setTickOrient(vec3(11, 0, 0), false);
		break;

	case k_bi_x:
		setTickOrient(vec3(1, 0, 0), true);
		break;

	case k_y:
		setTickOrient(vec3(0, 1, 0), false);
		break;

	case k_neg_y:
		setTickOrient(vec3(0, -1, 0), false);
		break;

	case k_bi_y:
		setTickOrient(vec3(0, 1, 0), true);
		break;

	case k_z:
		setTickOrient(vec3(0, 0, 1), false);
		break;

	case k_neg_z:
		setTickOrient(vec3(0, 0, -1), false);
		break;

	case k_bi_z:
		setTickOrient(vec3(0, 0, 1), true);
		break;

	default:
		assert(false);
		break;
	}
}