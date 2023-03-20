#pragma once
#include <memory>
#include "KtMatrix4.h"
#include "KtAABB.h"
#include "3d/KePrimitiveType.h"

class KcGpuBuffer;
class KcGlslProgram;
class KcVertexDeclaration;


// 封装待渲染对象：支持多vbo、多ibo和多实例

class KcRenderObject
{
public:

	using aabb_t = KtAABB<float, 3>;

	KcRenderObject(KePrimitiveType type) : type_(type) {}

	KcRenderObject(const KcRenderObject& rhs);


	std::shared_ptr<KcGlslProgram> shader() const {
		return prog_;
	}

	void setShader(std::shared_ptr<KcGlslProgram> prog) {
		prog_ = prog;
	}

	std::shared_ptr<KcGpuBuffer> vbo(unsigned idx) const {
		return vbos_[idx].buf;
	}

	std::shared_ptr<KcVertexDeclaration> vertexDecl(unsigned idx) const {
		return vbos_[idx].decl;
	}

	void pushVbo(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl);

	std::shared_ptr<KcGpuBuffer> ibo(unsigned idx) const {
		return ibos_[idx].buf;
	}

	void pushIbo(KePrimitiveType type, std::shared_ptr<KcGpuBuffer> ibo, unsigned count, unsigned start = 0) {
		ibos_.push_back({ type, ibo, count, start });
	}

	void pushIbo(std::shared_ptr<KcGpuBuffer> ibo, unsigned count, unsigned start = 0) {
		pushIbo(type_, ibo, count, start);
	}

	auto& projMatrix() const { return projMat_; }

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

	virtual KcRenderObject* clone() const;

	bool hasColor() const;

protected:

	void bindVbo_() const;
	void setUniforms_(const std::shared_ptr<KcGlslProgram>& shader) const;
	void drawVbo_() const;

	// 拷贝当前对象的属性到obj
	void cloneTo_(KcRenderObject& obj) const;

	void calcInst_();

protected:

	struct KpVbo_
	{
		std::shared_ptr<KcGpuBuffer> buf;
		std::shared_ptr<KcVertexDeclaration> decl;
	};

	struct KpIbo_
	{
		KePrimitiveType type; // 允许每个ibo使用不同的类型绘制
		std::shared_ptr<KcGpuBuffer> buf;
		unsigned count{ 0 };
		unsigned start{ 0 };
	};

	KePrimitiveType type_;
	std::shared_ptr<KcGlslProgram> prog_;
	std::vector<KpVbo_> vbos_;
	std::vector<KpIbo_> ibos_;
	float4x4<> projMat_;
	aabb_t clipBox_;
	float4 color_{ 1, 0, 0, 1 };
	unsigned instances_{ 0 }; // 实例数目
};
