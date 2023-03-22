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

	void setScale(point2f sz) { size_ = sz; }

	// ���ݴ��������ʼ��marker��vbo
	void setMarker(const KpMarker& marker);
	
	// ���ݴ��������ʼ��offset��vbo
	void setOffset(const point3f* pos, unsigned count);


private:

	void buildMarkerVbo_(const KpMarker& marker);

	void buildIbo_(unsigned vtxSize);

	static bool isSolidColor(const KpMarker& marker);

	static std::pair<const void*, unsigned> markerVtx_(int type);

private:
	point2f size_{ 1 };

	KpMarker marker_;
};
