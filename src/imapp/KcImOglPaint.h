#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include "opengl/KcRenderObject.h"


// 基于ImGui环境下OpenGL的KvPaint实现

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;

public:

	using super_::super_;

	void beginPaint() override;
	void endPaint() override;

	void drawLineStrip(point_getter fn, unsigned count) override;


private:

	void pushRenderObject_(KcRenderObject* obj);

private:
	std::vector<std::unique_ptr<KcRenderObject>> objs_;
};
