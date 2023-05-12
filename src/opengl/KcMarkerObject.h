#pragma once
#include "KcRenderObject.h"
#include "plot/KpContext.h"


class KcMarkerObject : public KcRenderObject
{
	using super_ = KcRenderObject;
	super_::pushVbo;

public:
	KcMarkerObject();

	void draw() const override;

	KcRenderObject* clone() const override;

	// 用来缩放marker大小，以保证渲染结果的像素尺寸正确
	void setScale(point3f s) { scale_ = s; }

	// 根据传入参数初始化marker的vbo
	void setMarker(const KpMarker& marker);
	
	// 设置各实例的位置
	void setInstPos(const point3f* pos, unsigned count);

	// 设置各实例的填充色
	// @clr等于null表示重置该属性
	void setInstColor(const color4f* clr);

	// 设置各实例的尺寸
	// @size等于null表示重置该属性
	void setInstSize(const float* size);

private:

	void buildMarkerVbo_();

	void buildIbo_(unsigned vtxSize);

	bool realShowOutline_() const;

private:
	point3f scale_{ 1 };

	KpMarker marker_;
};
