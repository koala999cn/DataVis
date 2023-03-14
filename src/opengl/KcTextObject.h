#pragma once
#include "KcRenderObject.h"


class KcTextObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:
	KcTextObject(int texId, int index = 0);

	void draw() const override;

	KcRenderObject* clone() const override;

private:
	int texId_{ 0 };
	int unitIdx_{ 0 };
};
