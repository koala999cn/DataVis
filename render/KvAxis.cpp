#include "KvAxis.h"


KvAxis::KvAxis(KeType type) 
{
	type_ = type;
	lower_ = 0, upper_ = 5;
	title_ = type == k_x ? "x" : type == k_y ? "y" : "z";
	visible_ = true;
	showAll();

	baselineSize_ = 1;
	tickSize_ = 1, tickLength_ = 1;
	subtickSize_ = 1, subtickLength_ = 1;
	tickSide_ = k_outter_side;

	baselineColor_ = QColor("black");
	tickColor_ = subtickColor_ = QColor("black");
	labelColor_ = titleColor_ = QColor("black");

	labelFont_, titleFont_; // TODO:
}