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

	void beginPaint() override;
	void endPaint() override;

	void drawPoint(const point3& pt) override;

	void drawPoints(point_getter fn, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;


	// 内部函数，由ImGui回调，以绘制renderList_保存的渲染对象
	void drawRenderList_();

private:

	point3 toNdc_(const point3& pt) const;
	
private:

	// render states stacks

	std::vector<rect_t> viewportHistList_;
	std::vector<rect_t> clipRectHistList_;
	std::vector<aabb_t> clipBoxHistList_;

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_; // -1表示未设置
	unsigned curClipBox_;

	// 使用tuple保存对以上3个堆栈的索引组
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned>;

	struct KpRenderList_
	{
		std::vector<std::unique_ptr<KcRenderObject>> objs; // 绘制plottables
		std::vector<std::function<void(void)>> fns; // 绘制其他点线
		std::vector<TextVbo> texts; // 绘制文本
	};

	// 按渲染状态排序的待渲染列表
	std::map<kRenderState_, KpRenderList_> renderList_;

	//std::vector<unsigned> glLists_; // 保存用来启动clipBox的显示列表id
	//unsigned clipBox_{ 0 }; // 保存当前的clipBox状态，对应于glLists_的索引
	                        // 可通过glCallList(glLists_[clipBox_])启用相应的clipBox，0表示禁用


private:
	KpRenderList_& currentRenderList();

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);
	void glClipPlane_(unsigned id);
	
	void pushTextVbo_(KpRenderList_& rl); // 生成绘制文本所需的vbo
};
