#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <map>
#include "opengl/KcRenderObject.h"


// 基于ImGui环境下OpenGL的KvPaint实现

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;
	using aabb_t = KtAABB<float_t, 3>;

public:

	struct TextVbo
	{
		point3f pos;
		point2f uv;
		color4f clr;
	};

	KcImOglPaint(camera_type& cam);

	void setViewport(const rect_t& vp) override;

	void pushClipRect(const rect_t& cr) override;
	void popClipRect() override;

	void enableClipBox(point3 lower, point3 upper) override;
	void disableClipBox() override;

	void enableDepthTest(bool b) override { depthTest_ = b; }
	bool depthTest() const override { return depthTest_; }

	void enableAntialiasing(bool b) override { antialiasing_ = b; }
	bool antialiasing() const override { return antialiasing_; }

	void beginPaint() override;
	void endPaint() override;

	void drawPoint(const point3& pt) override;

	void drawPoints(point_getter fn, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawGeom(vtx_decl_ptr decl, geom_ptr geom, bool fill, bool showEdge) override;

	void fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void grab(int x, int y, int width, int height, void* data) override;


	// 内部函数，由ImGui回调，以绘制renderList_保存的渲染对象
	void drawRenderList_();

private:

	point3 toNdc_(const point3& pt) const;
	
	void pushRenderObject_(KcRenderObject* obj);

private:

	// render states stacks

	std::vector<rect_t> viewportHistList_; // 视口列表
	std::vector<rect_t> clipRectHistList_; // 裁剪rect列表
	std::vector<aabb_t> clipBoxHistList_; // 三维空间裁剪box列表

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_{ unsigned(-1) }; // -1表示未设置
	unsigned curClipBox_{ unsigned(-1) };
	bool depthTest_{ false }; // 启动深度测试？
	bool antialiasing_{ false };

	// [0]: viewport idx
	// [1]: clipRect idx
	// [2]: clipBox idx
	// [3]: depth test
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned, bool>;

	struct KpRenderList_
	{
		std::vector<std::unique_ptr<KcRenderObject>> objs; // 绘制plottables
		std::vector<std::function<void(void)>> fns; // 绘制其他点线
		std::vector<TextVbo> texts; // 绘制文本
	};

	// 按渲染状态排序的待渲染列表
	std::map<kRenderState_, KpRenderList_> renderList_;

private:
	KpRenderList_& currentRenderList();

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);
	void glClipPlane_(unsigned id);
	
	void pushTextVbo_(KpRenderList_& rl); // 生成绘制文本所需的vbo
};
