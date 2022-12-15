#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include "opengl/KcRenderObject.h"


// 基于ImGui环境下OpenGL的KvPaint实现

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;

public:

	using super_::super_;

	void beginPaint() override;
	void endPaint() override;

	void drawPoint(const point3& pt) override;

	void drawPoints(point_getter fn, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;


private:

	void pushRenderObject_(KcRenderObject* obj);

	void setGlViewport_(const rect_t& rc);

private:
	std::vector<std::unique_ptr<KcRenderObject>> objs_;
	std::vector<std::function<void(void)>> fns_;
};
