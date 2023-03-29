#pragma once
#include "KcRenderObject.h"


class KcTextObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:
	KcTextObject(int texId, int index = 0);

	void draw() const override;

	KcRenderObject* clone() const override;

	void setScale(point3f s) { scale_ = s; }

	void setBufferData(const point3f* anchors, const point4f* offset, const point4f* uvs, unsigned count);

private:
	int texId_{ 0 };
	int unitIdx_{ 0 };

	point3f scale_{ 1 };
};
