#pragma once
#include "KcRenderObject.h"

// 统一3d对象的edge绘制
// NB: 在fill情况下，edge只能单色绘制。无fill情况下啊，edge使用fill配色进行绘制

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
	std::shared_ptr<KcGlslProgram> edgeShader_; // 用于绘制edge的shader
	float edgeWidth_{ 0.75 };
	int edgeStyle_{ 0 };
	float4 edgeColor_{ 0, 0, 0, 1 }; // 当主色被fill使用时，使用该色描边
	bool fill_{ true }, edge_{ true }; // 是否填充？是否描边？
};
