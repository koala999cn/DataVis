#pragma once
#include "KcRenderObject.h"

// ͳһ3d�����edge����
// NB: ��fill����£�edgeֻ�ܵ�ɫ���ơ���fill����°���edgeʹ��fill��ɫ���л���

class KcEdgedObject : public KcRenderObject
{
	using super_ = KcRenderObject;

public:

	KcEdgedObject(KePrimitiveType type);

	std::shared_ptr<KcGlslProgram> edgeShader() const {
		return edgeShader_;
	}

	void setEdgeShader(std::shared_ptr<KcGlslProgram> prog) {
		edgeShader_ = prog;
	}

	float edgeWidth() const { return edgeWidth_; }
	void setEdgeWidth(float w) { edgeWidth_ = w; }

	int edgeStyle() const { return edgeStyle_; }
	void setEdgeStyle(int style) { edgeStyle_ = style; }

	const float4& edgeColor() const { return edgeColor_; }
	void setEdgeColor(const float4& clr) { edgeColor_ = clr; }

	void setFilled(bool b) { fill_ = b; }
	void setEdged(bool b) { edge_ = b; }

	void draw() const override;

private:
	std::shared_ptr<KcGlslProgram> edgeShader_; // ���ڻ���edge��shader
	float edgeWidth_{ 0.75 };
	int edgeStyle_{ 0 };
	float4 edgeColor_{ 0, 0, 0, 1 }; // ����ɫ��fillʹ��ʱ��ʹ�ø�ɫ���
	bool fill_{ true }, edge_{ true }; // �Ƿ���䣿�Ƿ���ߣ�
};
