#pragma once
#include "KvPlot.h"
#include "KtMatrix4.h"
#include "KvRenderable.h"


// ��άplot�ĳ�����

class KvPlot3d : public KvPlot
{
public:
	using float_t = typename KvRenderable::float_t;
	using aabb_t = KtAABB<float_t>;
	using mat4 = KtMatrix4<float_t>;
	using point3 = KtPoint<float_t, 3>;
	using vec3 = point3;
	using quat = KtQuaternion<float_t>;
	
	KvPlot3d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord);

	/// ����ӿ�

	virtual mat4 viewMatrix() const = 0;
	virtual void setViewMatrix(const mat4&) = 0;

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

	bool ortho() const { return ortho_; }
	bool& ortho() { return ortho_; }

	bool isotropic() const { return isotropic_; }
	bool& isotropic() { return isotropic_; }

	float_t zoom() const { return zoom_; }
	float_t& zoom() { return zoom_; }

	const point3& scale() const { return scale_; }
	point3& scale() { return scale_; }

	const point3& shift() const { return shift_; }
	point3& shift() { return shift_; }

	const quat& orient() const { return orient_; }
	quat& orient() { return orient_; }

protected:
	void autoProject_() override;
	
protected:

	bool ortho_{ true }; // ����ͶӰ vs. ͸��ͶӰ
	
	// ���²������ڵ��������modelview����
	float_t zoom_{ 1 };
	point3 scale_{ 1, 1, 1 }; 
	point3 shift_{ 0, 0, 0 };
	quat orient_{ 0, 0, 0, 1 }; // ������ķ�λ

	bool isotropic_{ false }; // ��true���򱣳�����ϵ�ĵȱ��ԣ�������ĵ�Ԫ���ȱ���һ��
	                          // ��false�������ȿ��ǲ��������ԣ�����ϵ�ĳ�ʼ͸�ӽ��ʼ��Ϊ�����Σ�������ͨ��scale_�����������죩
};
