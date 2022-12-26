#pragma once
#include <memory>
#include "KtMatrix4.h"
#include "KtAABB.h"
#include "3d/KePrimitiveType.h"

class KcGpuBuffer;
class KcGlslProgram;
class KcVertexDeclaration;


// 封装待渲染对象：包括vbo、shader等

class KcRenderObject
{
public:

	using aabb_t = KtAABB<float, 3>;

	KcRenderObject(KePrimitiveType type) : type_(type) {}

	KcRenderObject(const KcRenderObject& rhs) 
		: type_(rhs.type_), prog_(rhs.prog_), vbo_(rhs.vbo_), ibo_(rhs.ibo_), 
		vtxDecl_(rhs.vtxDecl_), indexCount_(rhs.indexCount_) {}

	void setShader(std::shared_ptr<KcGlslProgram> prog) {
		prog_ = prog;
	}

	void setVBO(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl) {
		vbo_ = vbo, vtxDecl_ = vtxDecl;
	}

	void setIBO(std::shared_ptr<KcGpuBuffer> ibo, unsigned idxCount) {
		ibo_ = ibo, indexCount_ = idxCount;
	}

	void setProjMatrix(const float4x4<>& projMat) {
		projMat_ = projMat;
	}

	void setClipBox(const aabb_t& clipBox) {
		clipBox_ = clipBox;
	}

	void setColor(const float4& clr) {
		color_ = clr;
	}

	virtual void draw() const;

protected:
	KePrimitiveType type_;
	std::shared_ptr<KcGlslProgram> prog_;
	std::shared_ptr<KcGpuBuffer> vbo_, ibo_;
	std::shared_ptr<KcVertexDeclaration> vtxDecl_;
	unsigned indexCount_{ 0 };
	float4x4<> projMat_;
	aabb_t clipBox_;
	float4 color_{ 1, 0, 0, 1 };
};
