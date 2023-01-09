#pragma once
#include "KcRenderObject.h"

// 统一3d对象的edge绘制
// NB: edge只能单色绘制

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

	void setFilled(bool b) { fill_ = b; }
	void setEdged(bool b) { edge_ = b; }

	void draw() const override;

private:
	std::shared_ptr<KcGlslProgram> edgeShader_; // 用于绘制edge的shader
	float edgeWidth_{ 0.75 };
	bool fill_{ true }, edge_{ true }; // 是否填充？是否描边？
};
