#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix4.h"
#include "KtQuaternion.h"
#include "KtAABB.h"


template<typename KREAL, bool ROW_MAJOR = true>
class KtCamera
{
public:
	using point2 = KtPoint<KREAL, 2>;
	using point3 = KtPoint<KREAL, 3>;
	using vec3 = KtVector3<KREAL>;
	using vec4 = KtVector4<KREAL>;
	using mat3 = KtMatrix3<KREAL, ROW_MAJOR>;
	using mat4 = KtMatrix4<KREAL, ROW_MAJOR>;
	using quat = KtQuaternion<KREAL>;
	using rect = KtAABB<KREAL, 2>;

	KtCamera();

	const mat4& viewMatrix() const { return viewMatrix_; }
	mat4& viewMatrix() { return viewMatrix_; }

	const mat4& projMatrix() const { return projMatrix_; }
	mat4& projMatrix() { return projMatrix_; }

	const rect& viewport() const { return vp_; }
	rect& viewport() { return vp_; }

	vec3 getCameraPos() const;
	quat getCameraOrient() const;

	// ��world�����/ʸ��in����ͶӰ����ת����viewport����ϵ
	bool project(const vec4& in, vec4& out) const;

	// ��viewport�����in���з�ͶӰ����ת����world����ϵ
	// ����in.z()Ϊ�淶�������ֵ��ȡ[0, 1]����Ӧ[znear, zfar]
	bool unproject(const vec3& in, vec4& out) const;

	// ����ray������
	// ray��ԭ���ͨ��getCameraPos��ȡ
	vec3 screenPointToRay(const point2& pt) const;
	
	point2 worldToViewport(const vec4& in) const {
		vec4 out;
		project(in, out);
		return { out.x(), out.y() };
	}

	point2 worldToScreen(const vec4& in) {
		return switchViewportAndScreen(worldToViewport(in));
	}

	// viewport���½�Ϊ(0, 0)�㣬screen���Ͻ�Ϊ(0, 0)��
	// �淶�������£�y(scr*) = 1 - y(vp*)
	// ��Ļ�����£�y(scr) = y0 + y(scr*) * h 
	//                    = y0 + (1-y(vp*)) * h = y0 + h - y(vp*) * h 
	// ��ͼ�����£�y(vp) = y0 + y(vp*) * h ==> y(vp*) = (y(vp) - y0) / h
	// ���ϣ�y(scr) = y0 + h - y(vp) + y0 = 2 * y0 + h - y(vp)
	point2 switchViewportAndScreen(const point2& pt) const {

		return { pt.x(), 2 * vp_.lower().y() + vp_.height() - pt.y() };
	}

	KREAL width() const { return vp_.width() }
	KREAL height() const { return vp_.height(); }

protected:

	// Viewing window. 
	rect vp_;

	// view matrix
	mat4 viewMatrix_; // viewMatrix_ * ��������ϵvec4 = ���������ϵvec4

	// standard projection matrix
	mat4 projMatrix_;
};


template<typename KREAL, bool ROW_MAJOR>
KtCamera<KREAL, ROW_MAJOR>::KtCamera()
{
	viewMatrix_ = projMatrix_ = mat4::identity();
}


template<typename KREAL, bool ROW_MAJOR>
bool KtCamera<KREAL, ROW_MAJOR>::project(const vec4& in, vec4& out) const
{
	out = projMatrix_ * viewMatrix_ * in;

	if (out.w() == 0.0f)
		return false;

	// ��ʱoutΪ���᷶ΧΪ[-1, +1]�Ĺ淶������
	out.homogenize();

	// map to range [0, 1]
	out.x() = out.x() * 0.5f + 0.5f;
	out.y() = out.y() * 0.5f + 0.5f;
	out.z() = out.z() * 0.5f + 0.5f;

	// map to viewport
	out.x() = out.x() * vp_.width() + vp_.lower().x();
	out.y() = out.y() * vp_.height() + vp_.lower().y();
	return true;
}


template<typename KREAL, bool ROW_MAJOR>
bool KtCamera<KREAL, ROW_MAJOR>::unproject(const vec3& in, vec4& out) const
{
	vec4 v(in);

	// map from viewport to [0, 1]
	v.x() = (v.x() - vp_.lower().x()) / vp_.width();
	v.y() = (v.y() - vp_.lower().y()) / vp_.height();

	// map to range [-1, +1]
	v.x() = v.x() * 2.0f - 1.0f;
	v.y() = v.y() * 2.0f - 1.0f;
	v.z() = v.z() * 2.0f - 1.0f;

	mat4 inverse = (projMatrix_ * viewMatrix_).getInverse();

	v = inverse * v;
	if (v.w() == 0.0)
		return false;

	out = v / v.w();
	return true;
}


template<typename KREAL, bool ROW_MAJOR>
KtVector3<KREAL> KtCamera<KREAL, ROW_MAJOR>::screenPointToRay(const point2& pt) const
{
	auto vpt = switchViewportAndScreen(pt);
	vec4 out;
	unproject({ vpt.x(), vpt.y(), 0 }, out);
	return (vec3(out.x(), out.y(), out.z()) - getCameraPos()).normalize();
}


template<typename KREAL, bool ROW_MAJOR>
KtVector3<KREAL> KtCamera<KREAL, ROW_MAJOR>::getCameraPos() const
{
	return viewMatrix_.getTranslation();
}


template<typename KREAL, bool ROW_MAJOR>
KtQuaternion<KREAL> KtCamera<KREAL, ROW_MAJOR>::getCameraOrient() const
{
	return viewMatrix_.getRotation(); // TODO:
}
