#pragma once
#include "KcImPaint.h"


// ����ImGui������OpenGL��KvPaintʵ��

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;

public:

	using super_::super_;

	void drawLineStrip(point_getter fn, unsigned count) override;

};
