#pragma once
#include "KtColor.h"
#include <functional>
#include "KvRenderable.h"
#include "KpContext.h"
#include "KvGeometry.h"
#include "KcVertexDeclaration.h"
#include "KtMatrix4.h"
#include "KtQuaternion.h"


// 一个简单的3d绘制接口

class KvPaint
{
public:
	using color_t = color4f;
	using float_t = typename KvRenderable::float_t;
	using rect_t = KtAABB<float_t, 2>;
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using point4 = KtPoint<float_t, 4>;
	using mat4 = KtMatrix4<float_t>;
	using point_getter = std::function<point3(unsigned)>;
	using geom_ptr = std::shared_ptr<KvGeometry>;
	using vtx_decl_ptr = std::shared_ptr<KcVertexDeclaration>;

	virtual void beginPaint() = 0;
	virtual void endPaint() = 0;

	virtual rect_t viewport() const = 0;
	virtual void setViewport(const rect_t& vp) = 0;

	virtual void pushClipRect(const rect_t& cr) = 0;
	virtual void popClipRect() = 0;

	// 改变model变换矩阵，即局部坐标系
	virtual void pushLocal(const mat4& mat) = 0;
	virtual void popLocal() = 0;

	enum KeCoordType
	{
		k_coord_local,
		k_coord_world, // 世界坐标系，即绘图坐标系
		k_coord_local_screen,
		k_coord_screen, // 屏幕坐标系
		k_coord_ndc
	};

	// 改变当前坐标系类型
	virtual void pushCoord(KeCoordType type) = 0;
	virtual void popCoord() = 0;
	virtual KeCoordType currentCoord() const = 0;

	// 启用由3维坐标系的6个坐标平面构成的clipPlane
	virtual void enableClipBox(point3 lower, point3 upper) = 0;
	virtual void disableClipBox() = 0;

	// 启用时，作用类似于glPolygonOffset(1, 1)，可用于防止z-fighting
	virtual void enablePolygonOffset(bool b) = 0;

	virtual void enableDepthTest(bool b) = 0; // 开启/关闭深度测试
	virtual bool depthTest() const = 0; // 是否开启了深度测试

	virtual void enableAntialiasing(bool b) = 0; // 开启/关闭反走样
	virtual bool antialiasing() const = 0; // 是否开启了反走样

	virtual void enableFlatShading(bool b) = 0; // 开启/关闭flat着色模式
	virtual bool flatShading() const = 0; // 是否开启了flat着色模式

	// 光照支持
	virtual void enableLighting(bool b) = 0;
	virtual bool lighting() const = 0;

	// project local point/vector to screen point/vector
	virtual point4 project(const point4& pt) const = 0;

	// project screen point/vector to local point/vector
	virtual point4 unproject(const point4& pt) const = 0;

	virtual point4 localToWorld(const point4& pt) const = 0;

	virtual point4 worldToLocal(const point4& pt) const = 0;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setSecondaryColor(const color_t& clr) = 0;

	virtual void setMarkerSize(double size) = 0;

	virtual void setMarkerType(int type) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void setLineStyle(int style) = 0;

	virtual void setFilled(bool b) = 0;

	virtual void setEdged(bool b) = 0;

	// 对于批量绘制，比如drawGeom, drawLineStrip, drawMarkers等，返回一个可重用的指针对象（nullptr表示不支持重用）
	// 此对象指针可传递给redraw进行二次绘制，避免重构渲染对象
	// 返回nullptr表示当前指针无法直接重绘，须重构渲染对象，非空表示重绘成功，并须用户更新对象指针以便下次使用
	virtual void* redraw(void* obj) { return nullptr; } // 缺省不支持vbo复用

	virtual void drawMarker(const point3& pt) = 0;

	virtual void* drawMarkers(const point3 pts[], unsigned count);

	virtual void* drawMarkers(point_getter fn, unsigned count);

	using color_getter = std::function<color4f(unsigned)>;
	using size_getter = std::function<float(unsigned)>;
	virtual void* drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count) = 0;

	virtual void drawLine(const point3& from, const point3& to) = 0;

	virtual void* drawLineStrip(const point3 pts[], unsigned count);

	virtual void* drawLineStrip(point_getter fn, unsigned count);

	// 绘制多条线段集合
	virtual void* drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts) = 0;

	virtual void drawLineLoop(const point3 pts[], unsigned count);

	virtual void drawLineLoop(point_getter fn, unsigned count);

	virtual void drawRect(const point3& lower, const point3& upper);

	virtual void drawBox(const point3& lower, const point3& upper);

	virtual void fillTriangle(point3 pts[3]) = 0;

	virtual void fillTriangle(point3 pts[3], color_t clrs[3]) = 0;

	virtual void fillRect(const point3& lower, const point3& upper) = 0;

	virtual void fillQuad(point3 pts[4]);

	virtual void fillQuad(point3 pts[4], color_t clrs[4]);

	virtual void fillConvexPoly(point_getter fn, unsigned count) = 0;

	virtual void* fillBetween(point_getter line1, point_getter line2, unsigned count) = 0;

	// 实现文本在三维平面的绘制
	// @topLeft: 文本框的左上点位置
	// @hDir: 文字布局的水平方向
	// @vDir: 文字布局的垂直方向。文字按此方向从上到下书写
	virtual void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) = 0;

	// 该方法默认dir = {1, 0, 0}
	// topLeft点将根据anchor和align确定
	// @anchor: 文本框的锚点。文本框按align方式对齐于anchor
	virtual void drawText(const point3& anchor, const char* text, int align) = 0;

	// 批量绘制文本，返回可复用的渲染对象
	virtual void* drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing) = 0;

	virtual void* drawGeom(vtx_decl_ptr decl, geom_ptr geom) = 0;

	// 抓取渲染缓存区数据到data.
	// data大小 = rc.width() * rc.height * 4
	virtual void grab(int x, int y, int width, int height, void* data) = 0;

	// 一些尺寸计算函数

	virtual point2 textSize(const char* text) const = 0;

	/// 光照支持

	virtual point3 lightDirection() const = 0;
	virtual void setLightDirection(const point3&) = 0;

	virtual color3f lightColor() const = 0;
	virtual void setLightColor(const color3f&) = 0;

	virtual color3f ambientColor() const = 0;
	virtual void setAmbientColor(const color3f&) = 0;

	virtual color3f specularColor() const = 0;
	virtual void setSpecularColor(const color3f&) = 0;

	virtual float_t shininess() const = 0;
	virtual void setShininess(float_t) = 0;


	/// 一些便捷函数

	// 点投影
	point3 projectp(const point3& pt) const {
		auto r = project(point4(pt.x(), pt.y(), pt.z(), 1));
		assert(r.w() == 1);
		return { r.x(), r.y(), r.z() };
	}

	point3 unprojectp(const point3& pt) const {
		auto r = unproject(point4(pt.x(), pt.y(), pt.z(), 1));
		assert(r.w() == 1); // NB: 透视投影模式下，须对结果归一化
		return { r.x(), r.y(), r.z() };
	}

	point3 unprojectp(const point2& pt) const {
		auto r = unproject(point4(pt.x(), pt.y(), 0, 1));
		return { r.x(), r.y(), r.z() };
	}

	point3 localToWorldP(const point3& pt) const {
		auto r = localToWorld(point4(pt.x(), pt.y(), pt.z(), 1));
		return { r.x(), r.y(), r.z() };
	}

	point3 worldToLocalP(const point3& pt) const {
		auto r = worldToLocal(point4(pt.x(), pt.y(), pt.z(), 1));
		return { r.x(), r.y(), r.z() };
	}

	// 矢量投影
	point3 projectv(const point3& v) const {
		//auto r = project(point4(v.x(), v.y(), v.z(), 0));
		//return { r.x(), r.y(), r.z() };
		return projectp(v) - projectp(point3(0)); // TODO: 透视投影模式下，不能直接使用上述代码
	}

	point3 unprojectv(const point3& pt) const {
		//auto r = unproject(point4(pt.x(), pt.y(), pt.z(), 0));
		//return { r.x(), r.y(), r.z() };
		return unprojectp(pt) - unprojectp(point3(0)); // TODO: 透视投影模式下，不能直接使用上述代码
	}

	point3 unprojectv(const point2& pt) const {
		return unprojectv(point3(pt.x(), pt.y(), 0));
	}

	point3 localToWorldV(const point3& v) const {
		auto r = localToWorld(point4(v.x(), v.y(), v.z(), 0));
		return { r.x(), r.y(), r.z() };
	}

	point3 worldToLocalV(const point3& pt) const {
		auto r = worldToLocal(point4(pt.x(), pt.y(), pt.z(), 0));
		return { r.x(), r.y(), r.z() };
	}

	bool inScreenCoord() const {
		return currentCoord() == k_coord_screen || currentCoord() == k_coord_local_screen;
	}

	bool inLocalCoord() const {
		return currentCoord() == k_coord_local || currentCoord() == k_coord_local_screen;
	}

	void drawRect(const rect_t& rc) {
		drawRect(point3(rc.lower().x(), rc.lower().y(), 0),
			     point3(rc.upper().x(), rc.upper().y(), 0));
	}

	void drawRect(const point2& lower, const point2& upper) {
		drawRect(point3(lower.x(), lower.y(), 0), point3(upper.x(), upper.y(), 0));
	}

	void fillRect(const rect_t& rc) {
		fillRect(point3(rc.lower().x(), rc.lower().y(), 0),
			point3(rc.upper().x(), rc.upper().y(), 0));
	}

	void fillRect(const point2& lower, const point2& upper) {
		fillRect(point3(lower.x(), lower.y(), 0), point3(upper.x(), upper.y(), 0));
	}

	void fillQuad(point2 pts[4]) {
		point3 pt3s[4];
		for (int i = 0; i < 4; i++)
			pt3s[i] = { pts[i].x(), pts[i].y(), 0 };
		fillQuad(pt3s);
	}

	void fillQuad(point2 pts[4], color_t clrs[4]) {
		point3 pt3s[4];
		for (int i = 0; i < 4; i++)
			pt3s[i] = { pts[i].x(), pts[i].y(), 0 };
		fillQuad(pt3s, clrs);
	}

	void drawText(const point2& ach, const char* text, int align) {
		drawText(point3(ach.x(), ach.y(), 0), text, align);
	}

	void* drawGeomSolid(geom_ptr geom);

	void* drawGeomColor(geom_ptr geom);

	void apply(const KpPen& cxt);

	void apply(const KpBrush& cxt);

	void apply(const KpFont& cxt);

	void apply(const KpMarker& cxt);
};
