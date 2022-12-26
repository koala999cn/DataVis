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

	void beginPaint() override;
	void endPaint() override;

	void drawPoint(const point3& pt) override;

	void drawPoints(point_getter fn, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;


	// �ڲ���������ImGui�ص����Ի���renderList_�������Ⱦ����
	void drawRenderList_();

private:

	point3 toNdc_(const point3& pt) const;
	
private:

	// render states stacks

	std::vector<rect_t> viewportHistList_;
	std::vector<rect_t> clipRectHistList_;
	std::vector<aabb_t> clipBoxHistList_;

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_; // -1��ʾδ����
	unsigned curClipBox_;

	// ʹ��tuple���������3����ջ��������
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned>;

	struct KpRenderList_
	{
		std::vector<std::unique_ptr<KcRenderObject>> objs; // ����plottables
		std::vector<std::function<void(void)>> fns; // ������������
		std::vector<TextVbo> texts; // �����ı�
	};

	// ����Ⱦ״̬����Ĵ���Ⱦ�б�
	std::map<kRenderState_, KpRenderList_> renderList_;

	//std::vector<unsigned> glLists_; // ������������clipBox����ʾ�б�id
	//unsigned clipBox_{ 0 }; // ���浱ǰ��clipBox״̬����Ӧ��glLists_������
	                        // ��ͨ��glCallList(glLists_[clipBox_])������Ӧ��clipBox��0��ʾ����


private:
	KpRenderList_& currentRenderList();

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);
	void glClipPlane_(unsigned id);
	
	void pushTextVbo_(KpRenderList_& rl); // ���ɻ����ı������vbo
};
