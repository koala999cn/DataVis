#pragma once
#include "KcRenderObject.h"
#include "plot/KpContext.h"


class KcMarkerObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:
	KcMarkerObject();

	void draw() const override;

	KcRenderObject* clone() const override;

	// 用来缩放marker大小，以保证渲染结果的像素尺寸正确
	void setScale(point2f s) { scale_ = s; }

	// 根据传入参数初始化marker的vbo
	void setMarker(const KpMarker& marker);
	
	// 根据传入参数初始化offset的vbo
	void setOffset(const point3f* pos, unsigned count);


private:

	void buildMarkerVbo_();

	void buildIbo_(unsigned vtxSize);

	bool isSolidColor() const;

	static std::pair<const void*, unsigned> markerVtx_(int type);

private:
	point2f scale_{ 1 };

	KpMarker marker_;
};
