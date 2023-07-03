#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <map>
#include <string_view>
#include "opengl/KcRenderObject.h"


// ����ImGui������OpenGL��KvPaintʵ��

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;
	using aabb_t = KtAABB<float_t, 3>;

public:

	void setViewport(const rect_t& vp) override;

	void pushClipRect(const rect_t& cr, bool reset = true) override;
	void popClipRect() override;

	void enableClipBox(point3 lower, point3 upper) override;
	void disableClipBox() override;

	void beginPaint() override;
	void endPaint() override;

	void* redraw(void* obj) override;

	void drawMarker(const point3& pt) override;

	void* drawMarkers(point_getter fn, unsigned count) override;

	void* drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void* drawLineStrip(point_getter fn, unsigned count) override;

	void* drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts) override;

	void drawRect(const point3& lower, const point3& upper) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text) override;

	void* drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing) override;

	void* drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	void* fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void grab(int x, int y, int width, int height, void* data) override;

	// ��������
	KpMarker marker() const; // װ��marker����������

	// �ڲ���������ImGui�ص����Ի���renderList_�������Ⱦ����
	virtual void drawRenderList_();

protected:

	// ��pt�����Ͻ�Ϊԭ�㣩ת��Ϊgl-raster���꣨���½�Ϊԭ�㣩
	void glRasterPos_(point2& pt);

	void glRasterRect_(rect_t& rc);

	void glClipPlane_(unsigned id);

private:

	point3 toNdc_(const point3& pt) const;

	// ���Ƶ���
	void* drawPoints_(point_getter fn, unsigned count);

	void drawCircles_(point_getter fn, unsigned count);

	void drawQuadMarkers_(point_getter fn, unsigned count, const point2 quad[4]);

	void drawTriMarkers_(point_getter fn, unsigned count, const point2 tri[3]);

	struct KpRenderList_;
	KpRenderList_& currentRenderList();

	void pushRenderObject_(KpRenderList_& rl, KcRenderObject* obj);

	void pushRenderObject_(KcRenderObject* obj) {
		pushRenderObject_(currentRenderList(), obj);
	}

	KcRenderObject* lastRenderObject_();

	void configOglState_(); // ����opengl����

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);

	struct KpUvVbo;
	KcRenderObject* makeTextVbo_(std::vector<KpUvVbo>& vbo); // ���ɻ����ı������vbo
	void pushTextVbo_(KpRenderList_& rl); // ���ɻ����ı������vbo
	void pushColorVbo_(KpRenderList_& rl); // ���ɻ���tris��Ա�����vbo

	// ���³�Ա��KpRenderList_::trisд������

	void addLine_(const point3& pt0, const point3& pt1, const float4& clr); // ��tris����ʽ����ֱ��

	// ���͹����Σ�fn����͹����εĶ���
	void addConvexPolyFilled_(point_getter fn, unsigned count, const float4& clr);

	void addLineLoop_(point_getter fn, unsigned count, const float4& clr);

	void addRect_(const point3& lower, const point3& upper);
	void addRectFilled_(const point3& lower, const point3& upper);
	void addCircle_(const point3& center, float_t radius);
	void addCircleFilled_(const point3& center, float_t radius);

	void addQuadFilled_(const point3& p0, const point3& p1, const point3& p2, const point3& p3, const float4& clr);

	// NB: fillVtxʹ����ɫ���ƣ�outlineVtxʹ�ø�ɫ����
	void addMarkers_(point_getter fn, unsigned count, const point2* fillVtx, unsigned numFill,
		const point2* outlineVtx, unsigned numOutline);

	void addMarkers_(point_getter fn, unsigned count, const point2* fillVtx, unsigned numFill);

	// �ڵ�ǰ��Ⱦ�б��tris��Ա����count��KpColorVbo_����
	struct KpColorVbo_;
	KpColorVbo_* newColorVbo_(unsigned count);

	// posΪ��Ļ����
	void pushTrisSoild_(const point2 pos[], unsigned c, const float4& clr);

	// posΪ����ȵ���Ļ����
	void pushTrisSoild_(const float3 pos[], unsigned c, const float4& clr);

	// �洢render-object���Ա���֡����
	void saveObjList_();

	// ���ö���obj����Ⱦ����
	void syncObjProps_(KcRenderObject* obj);

	// @normToNdc: ��true����text����淶��ndc����ϵ
	void drawText_(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text, std::vector<KpUvVbo>& vbo, bool normToNdc);
	void drawText_(const point3& anchor, const char* text, int align, std::vector<KpUvVbo>& vbo, bool normToNdc);

	// pos�洢˳����Ļ���꣩: dx, dy, width, height
	// ����(dx, dy)Ϊ��i������quad���ĵ������text-rect���Ͻǵ�ƫ��
	// uv�洢˳�򣨹�һ�����꣩��u1, v1, u2, v2
	void pushTextData_(const std::string_view& text, std::vector<point4f>& pos, std::vector<point4f>& uvs) const;

	// �Զ����ɷ���vbo
	static void pushNormals_(KcRenderObject* obj);

	bool hasClipBox_() const;

private:

	// render states stacks

	std::vector<rect_t> viewportHistList_; // �ӿ��б�
	std::vector<rect_t> clipRectHistList_; // �ü�rect�б�
	std::vector<aabb_t> clipBoxHistList_; // ��ά�ռ�ü�box�б�

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_{ unsigned(-1) }; // -1��ʾδ����
	unsigned curClipBox_{ unsigned(-1) };

	// [0]: viewport idx
	// [1]: clipRect idx
	// [2]: clipBox idx
	// [3]: polygonOffset?
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned, bool>;

	struct KpColorVbo_
	{
		float3 pos;
		float4 clr;
	};

	struct KpUvVbo
	{
		float3 pos;
		float4 clr;
		point2f uv;
	};

	struct KpRenderList_
	{
		std::vector<std::shared_ptr<KcRenderObject>> objs; // ����plottables

		// NB: Ϊ�˼�����Ⱦ״̬������л�������Ԫ�ؾ�ʹ��ndc���꣬���ֻ��ͳһ��ͶӰ������Ϊ��λ�󼴿�
		std::vector<std::function<void(void)>> fns; // ���Ƶ���
		std::vector<KpUvVbo> texts; // �����ı�

		// NB: ����scatter2dͼʱ��ϣ����marker������fill��outline�����ջ���˳��ѵ�
		// ������������fill��������outline��EdgedObject����������������Ⱦ��������outline���໥����
		// ���⣬����legend��ͼ�ꡢ�߿�Ȼ���Ԫ�أ�Ҳ��Ҫ�ṩһ������ģʽ
		// Ϊ����������⣬����ImGuiʵ�֣���KpRenderList_�м�����tris��Ա��ר����������Ԫ����Ƹ���Ҫ��
		// ����line��rect��circle�ȣ�����posͳһʹ�á���Ļ���꡿��ͬʱΪ�˼򻯣���ʹ������
		std::vector<KpColorVbo_> tris; // �����������ι��ɵĸ���Ԫ�أ�������
	};

	// ����Ⱦ״̬����Ĵ���Ⱦ�б�
	std::map<kRenderState_, KpRenderList_> renderList_;

	std::map<void*, std::shared_ptr<KcRenderObject>> savedObjList_;
};
