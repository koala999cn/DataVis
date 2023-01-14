#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <map>
#include "opengl/KcRenderObject.h"


// ����ImGui������OpenGL��KvPaintʵ��

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

	void setFlatShading(bool b) override { flatShading_ = b; }
	bool flatShading() const override { return flatShading_; }
	
	void beginPaint() override;
	void endPaint() override;

	void drawMarker(const point3& pt) override;

	void drawMarkers(point_getter1 fn, unsigned count, bool outline) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter1 fn, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	void drawGeom(vtx_decl_ptr decl, geom_ptr geom, bool fill, bool showEdge) override;

	void fillBetween(point_getter1 line1, point_getter1 line2, unsigned count) override;

	void grab(int x, int y, int width, int height, void* data) override;

	bool inScreenCoord() const {
		return currentCoord() == k_coord_screen ||
			currentCoord() == k_coord_local_screen;
	}

	// �ڲ���������ImGui�ص����Ի���renderList_�������Ⱦ����
	void drawRenderList_();

private:

	point3 toNdc_(const point3& pt) const;
	
	void pushRenderObject_(KcRenderObject* obj);

	KcRenderObject* lastRenderObject_();

	void drawPoints_(point_getter1 fn, unsigned count);

	void drawCircles_(point_getter1 fn, unsigned count, bool outline);

	void drawQuadMarkers_(point_getter1 fn, unsigned count, const point2 quad[4], bool outline);

	void drawTriMarkers_(point_getter1 fn, unsigned count, const point2 tri[3], bool outline);


private:

	// render states stacks

	std::vector<rect_t> viewportHistList_; // �ӿ��б�
	std::vector<rect_t> clipRectHistList_; // �ü�rect�б�
	std::vector<aabb_t> clipBoxHistList_; // ��ά�ռ�ü�box�б�

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_{ unsigned(-1) }; // -1��ʾδ����
	unsigned curClipBox_{ unsigned(-1) };
	bool depthTest_{ false }; // ������Ȳ��ԣ�
	bool antialiasing_{ false };
	bool flatShading_{ false };

	// [0]: viewport idx
	// [1]: clipRect idx
	// [2]: clipBox idx
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned>;

	struct KpRenderList_
	{
		std::vector<std::unique_ptr<KcRenderObject>> objs; // ����plottables
		std::vector<std::function<void(void)>> fns; // ������������
		std::vector<TextVbo> texts; // �����ı�
	};

	// ����Ⱦ״̬����Ĵ���Ⱦ�б�
	std::map<kRenderState_, KpRenderList_> renderList_;

private:
	KpRenderList_& currentRenderList();

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);
	void glClipPlane_(unsigned id);
	
	void pushTextVbo_(KpRenderList_& rl); // ���ɻ����ı������vbo
};
