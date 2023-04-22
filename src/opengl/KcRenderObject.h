#pragma once
#include <memory>
#include "KtMatrix4.h"
#include "KtAABB.h"
#include "plot/KtColor.h"
#include "3d/KePrimitiveType.h"

class KcGpuBuffer;
class KcGlslProgram;
class KcVertexDeclaration;


// ��װ����Ⱦ����֧�ֶ�vbo����ibo�Ͷ�ʵ��

class KcRenderObject
{
public:

	using aabb_t = KtAABB<float, 3>;

	KcRenderObject(KePrimitiveType type) : type_(type) {}

	KcRenderObject(const KcRenderObject& rhs);

	KePrimitiveType type() const { return type_; }

	std::shared_ptr<KcGlslProgram> shader() const {
		return prog_;
	}

	void setShader(std::shared_ptr<KcGlslProgram> prog) {
		prog_ = prog;
	}

	unsigned vboCount() const { return vbos_.size(); }

	std::shared_ptr<KcGpuBuffer> vbo(unsigned idx) const {
		return vbos_[idx].buf;
	}

	std::shared_ptr<KcVertexDeclaration> vertexDecl(unsigned idx) const {
		return vbos_[idx].decl;
	}

	void pushVbo(std::shared_ptr<KcGpuBuffer> vbo, std::shared_ptr<KcVertexDeclaration> vtxDecl);

	unsigned iboCount() const { return ibos_.size(); }

	std::shared_ptr<KcGpuBuffer> ibo(unsigned idx) const {
		return ibos_[idx].buf;
	}

	// ��idx��ibo����������
	unsigned iboSize(unsigned idx) const {
		return ibos_[idx].count;
	}

	void pushIbo(KePrimitiveType type, std::shared_ptr<KcGpuBuffer> ibo, unsigned count) {
		ibos_.push_back({ type, ibo, count });
	}

	void pushIbo(std::shared_ptr<KcGpuBuffer> ibo, unsigned count) {
		pushIbo(type_, ibo, count);
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

	void setNormalMatrix(const float4x4<>& normalMat) {
		normalMat_ = normalMat;
	}

	void setLightDir(const float3& dir) {
		lightDir_ = dir;
		lightDir_.normalize();
	}

	void setLightColor(const color3f& clr) { lightColor_ = clr; }

	void setAmbientColor(const color3f& clr) { ambientColor_ = clr; }

	void setSpecularColor(const color3f& clr) { specularColor_ = clr; }

	void setShininess(float_t s) { shininess_ = s; }

	void setEyePos(const float3& pos) { eyePos_ = pos; }

	virtual void draw() const;

	virtual KcRenderObject* clone() const;

	bool hasColor(bool enableTest) const;
	bool hasUV(bool enableTest) const;
	bool hasNormal(bool enableTest) const;
	bool hasInst(bool enableTest) const;

	bool hasAttribute(int semantic, bool enableTest) const;

	void enableAttribute(int semantic, bool b);

protected:

	void bindVbo_() const;
	void setUniforms_(const std::shared_ptr<KcGlslProgram>& shader) const;
	void drawVbo_() const;

	// ������ǰ��������Ե�obj
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
		KePrimitiveType type; // ����ÿ��iboʹ�ò�ͬ�����ͻ���
		std::shared_ptr<KcGpuBuffer> buf;
		unsigned count{ 0 };
	};

	KePrimitiveType type_;
	std::shared_ptr<KcGlslProgram> prog_;
	std::vector<KpVbo_> vbos_;
	std::vector<KpIbo_> ibos_;
	float4x4<> projMat_;
	aabb_t clipBox_;
	float4 color_{ 1, 0, 0, 1 };
	unsigned instances_{ 0 }; // ʵ����Ŀ

	float4x4<> normalMat_;
	float3 lightDir_; // �������꣬ȷ���ѹ�һ��
	color3f lightColor_;
	color3f ambientColor_;
	color3f specularColor_;
	float shininess_;
	float3 eyePos_;
};
