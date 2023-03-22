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

	// ��������marker��С���Ա�֤��Ⱦ��������سߴ���ȷ
	void setScale(point2f s) { scale_ = s; }

	// ���ݴ��������ʼ��marker��vbo
	void setMarker(const KpMarker& marker);
	
	// ���ݴ��������ʼ��offset��vbo
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
