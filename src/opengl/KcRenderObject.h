#pragma once
#include <memory>
#include "KtMatrix4.h"
#include "KtAABB.h"
#include "3d/KePrimitiveType.h"

class KcGpuBuffer;
class KcGlslProgram;
class KcVertexDeclaration;


// ��װ����Ⱦ���󣺰���vbo��shader��

class KcRenderObject
{
public:

	using aabb_t = KtAABB<float, 3>;

	KcRenderObject(KePrimitiveType type, std::shared_ptr<KcGlslProgram> prog)
		: type_(type), prog_(prog) {}

	KcRenderObject(const KcRenderObject& rhs) 
		: type_(rhs.type_), prog_(rhs.prog_), vbo_(rhs.vbo_), vtxDecl_(rhs.vtxDecl_) {}

	void setVbo(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl) {
		vbo_ = vbo, vtxDecl_ = vtxDecl;
	}

	void setProjMatrix(const float4x4<>& projMat) {
		projMat_ = projMat;
	}

	void setClipBox(const aabb_t& clipBox) {
		clipBox_ = clipBox;
	}

	virtual void draw() const;

protected:
	KePrimitiveType type_;
	std::shared_ptr<KcGlslProgram> prog_;
	std::shared_ptr<KcGpuBuffer> vbo_;
	std::shared_ptr<KcVertexDeclaration> vtxDecl_;
	float4x4<> projMat_;
	aabb_t clipBox_;
};
