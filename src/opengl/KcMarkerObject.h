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

	// ��������marker��С���Ա�֤��Ⱦ��������سߴ���ȷ
	void setScale(point3f s) { scale_ = s; }

	// ���ݴ��������ʼ��marker��vbo
	void setMarker(const KpMarker& marker);
	
	// ���ø�ʵ����λ��
	void setInstPos(const point3f* pos, unsigned count);

	// ���ø�ʵ�������ɫ
	// @clr����null��ʾ���ø�����
	void setInstColor(const color4f* clr);

	// ���ø�ʵ���ĳߴ�
	// @size����null��ʾ���ø�����
	void setInstSize(const float* size);

private:

	void buildMarkerVbo_();

	void buildIbo_(unsigned vtxSize);

	bool realShowOutline_() const;

private:
	point3f scale_{ 1 };

	KpMarker marker_;
};
