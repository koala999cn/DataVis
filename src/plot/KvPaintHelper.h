#pragma once
#include "KvPaint.h"
#include "3d/KtCamera.h"


// paint的一个缺省实现（半成品，仅实现通用功能）

class KvPaintHelper : public KvPaint
{
public:
	using camera_type = KtCamera<float_t>;

	camera_type& camera() { return cam_; }
	const camera_type& camera() const { return cam_; }

	void enablePolygonOffset(bool b) override { polygonOffset_ = b; }

	void enableDepthTest(bool b) override { depthTest_ = b; }
	bool depthTest() const override { return depthTest_; }

	void enableAntialiasing(bool b) override { antialiasing_ = b; }
	bool antialiasing() const override { return antialiasing_; }

	void enableFlatShading(bool b) override { flatShading_ = b; }
	bool flatShading() const override { return flatShading_; }

	void enableLighting(bool b) override { lighting_ = b; }
	bool lighting() const override { return lighting_; }

	void beginPaint() override;
	void endPaint() override;

	rect_t viewport() const override;
	void setViewport(const rect_t& vp) override;

	void pushClipRect(const rect_t& cr) override;
	void popClipRect() override;

	void pushLocal(const mat4& mat) override;
	void popLocal() override;

	void pushCoord(KeCoordType type) override;
	void popCoord() override;
	KeCoordType currentCoord() const override;

	point4 project(const point4& worldPt) const override;

	point4 unproject(const point4& pt) const override;

	point4 localToWorld(const point4& pt) const override;

	point4 worldToLocal(const point4& pt) const override;

	void setColor(const color_t& clr) override;
	color_t color() const override { return clr_; }

	void setSecondaryColor(const color_t& clr) override;
	color_t secondaryColor() const override { return secondaryClr_; }

	void setMarkerSize(float_t size) override;
	double markerSize() const override { return markerSize_; }

	void setMarkerType(int type) override;
	int markerType() const override { return markerType_; }

	void setLineWidth(float_t width) override;

	void setLineStyle(int style) override;

	void setFilled(bool b) override;
	bool filled() const override { return filled_; }

	void setEdged(bool b) override;
	bool edged() const override { return edged_; }

	point3 lightDirection() const override { return lightDir_; }
	void setLightDirection(const point3& dir) override { lightDir_ = dir; }

	color3f lightColor() const override { return lightColor_; }
	void setLightColor(const color3f& clr) override { lightColor_ = clr; }

	color3f ambientColor() const override { return ambientColor_; }
	void setAmbientColor(const color3f& clr) override { ambientColor_ = clr; }

	color3f specularColor() const override { return specularColor_; }
	void setSpecularColor(const color3f& clr) override { specularColor_ = clr; }

	float_t shininess() const override { return shininess_; }
	void setShininess(float_t s) override { shininess_ = s; }

protected: // TODO: private

	camera_type cam_; // 依托KtCamera实现各类坐标变换

	bool polygonOffset_{ false };
	bool depthTest_{ false }; // 启动深度测试？
	bool antialiasing_{ false };
	bool flatShading_{ false };
	bool lighting_{ false };

	color_t clr_{ 0, 0, 0, 1 };
	color_t secondaryClr_{ 0, 0, 0, 0 };
	float_t lineWidth_{ 1 };
	int lineStyle_{ 0 };
	float_t markerSize_{ 1 };
	int markerType_{ 0 };
	bool filled_{ true }, edged_{ false };

	point3 lightDir_{ -1., -1., -1. };
	color3f lightColor_{ 1.f, 1.f, 1.f };
	color3f ambientColor_{ 0.3f, 0.3f, 0.3f };
	color3f specularColor_{ 0.5f, 0.5f, 0.5f };
	float_t shininess_{ 15 };

	std::vector<int> coords_{ k_coord_local }; // 坐标系堆栈
};
