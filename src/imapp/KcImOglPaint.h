#pragma once
#include "KcImPaint.h"


// 基于ImGui环境下OpenGL的KvPaint实现

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;

public:

	using super_::super_;

	void drawLineStrip(point_getter fn, unsigned count) override;

};
