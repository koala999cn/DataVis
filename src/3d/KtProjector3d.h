#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix4.h"
#include "KtQuaternion.h"
#include "KtAABB.h"
#include <optional>


// 主要在给定视图矩阵、透视矩阵和视口条件下，实现2类功能：
//   1. 各类坐标系的转换
//   2. 获取摄像机的参数

// 总共实现7类坐标系的互换
//   1. 局部坐标系(local)，也称物体坐标系
//   2. 世界坐标系(world)，local通过模型矩阵(model)转换为world
//   3. 摄像坐标系(eye)，world通过视图矩阵(view)转换为eye
//   4. 裁剪坐标系(clip)，eye通过透视矩阵(proj)转换为clip
//   5. 标准坐标系(NDC)，clip通过规范化(各元素除以w)转换为NDC，坐标范围从[-1, -1, -1]到[1, 1, 1]
//   6. 视口坐标系(viewport)，坐标范围从[0, 0]到[1, 1]
//   7. 屏幕坐标系(screen)，即视窗坐标

template<typename KREAL, bool ROW_MAJOR = true>
class KtProjector
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

	void pushLocal(const mat4& mat) {
		localMatStack_.push_back(localMatStack_.empty() ? mat :
			localMatStack_.back() * mat);
		resetModelRelatedMats_();
	}

	void popLocal() {
		localMatStack_.pop_back();
		resetModelRelatedMats_();
	}

	const mat4& viewMatrix() const { return viewMat_; }
	mat4& viewMatrix() { return viewMat_; }

	const mat4& projMatrix() const { return projMat_; }
	mat4& projMatrix() { return projMat_; }

	// get the model-view matrix
	const mat4& getMvMat() const;

	// get the model-view-projection matrix
	const mat4& getMvpMat() const;

	// 返回法向变换矩阵 
	// 等于model-view矩阵的逆的转置，即=(mv(-1))T
	mat4 getNormalMatrix() const;

	const rect& viewport() const { return vp_; }
	void setViewport(const rect& vp);

	vec3 getEyePos() const;
	quat getEyeOrient() const;

	// 更新与投影有关的变换矩阵
	// NOTE: 重置viewMat, projMat和viewport之后，须在调用坐标转换方法之前调用该函数
	void updateProjectMatrixs();

	vec4 localToWorld(const vec4& pt) const { return localMatStack_.empty() ? pt : localMatStack_.back() * pt; }
	vec4 localToEye(const vec4& pt) const { return getMvMat() * pt; }
	vec4 localToClip(const vec4& pt) const { return getMvpMat() * pt; }
	vec4 localToNdc(const vec4& pt) const { return clipToNdc(localToClip(pt)); }
	vec4 localToViewport(const vec4& pt) const { return clipToViewport(localToClip(pt)); }
	vec4 localToScreen(const vec4& pt) const { return clipToScreen(localToClip(pt)); }

	vec4 worldToLocal(const vec4& pt) const { return localMatStack_.empty() ? pt : getMMatR_() * pt; }
	vec4 worldToEye(const vec4& pt) const { return viewMat_ * pt; }
	vec4 worldToClip(const vec4& pt) const { return vpMat_ * pt; }
	vec4 worldToNdc(const vec4& pt) const { return clipToNdc(worldToClip(pt)); }
	vec4 worldToViewport(const vec4& pt) const { return clipToViewport(worldToClip(pt)); }
	vec4 worldToScreen(const vec4& pt) const { return wsMat_ * pt; }

	vec4 eyeToLocal(const vec4& pt) const { return getMvMatR_() * pt; }
	vec4 eyeToWorld(const vec4& pt) const { return getViewMatR_() * pt; }
	vec4 eyeToClip(const vec4& pt) const { return projMat_ * pt; }
	vec4 eyeToNdc(const vec4& pt) const { return clipToNdc(eyeToClip(pt)); }
	vec4 eyeToViewport(const vec4& pt) const { return clipToViewport(eyeToClip(pt)); }
	vec4 eyeToScreen(const vec4& pt) const { return clipToScreen(eyeToClip(pt)); }

	vec4 clipToLocal(const vec4& pt) const { return getMvpMatR_() * pt; }
	vec4 clipToWorld(const vec4& pt) const { return getVpMatR_() * pt; }
	vec4 clipToEye(const vec4& pt) const { return getProjMatR_() * pt; }
	vec4 clipToNdc(const vec4& pt) const { return vec4(pt).homogenize(); }

	vec4 clipToViewport(const vec4& pt) const { return ndcToViewport(clipToNdc(pt)); }
	vec4 clipToScreen(const vec4& pt) const { return ndcToScreen(clipToNdc(pt)); }

	vec4 ndcToLocal(const vec4& pt) const { return clipToLocal(ndcToClip(pt)); }
	vec4 ndcToWorld(const vec4& pt) const { return clipToWorld(ndcToClip(pt)); }
	vec4 ndcToEye(const vec4& pt) const { return clipToEye(ndcToClip(pt)); }
	vec4 ndcToClip(const vec4& pt) const { return pt; }
	vec4 ndcToViewport(const vec4& pt) const { return nvMat_ * pt; }
	vec4 ndcToScreen(const vec4& pt) const {  return nsMat_ * pt; }

	vec4 viewportToLocal(const vec4& pt) const { return ndcToLocal(viewportToNdc(pt)); }
	vec4 viewportToWorld(const vec4& pt) const { return ndcToWorld(viewportToNdc(pt)); }
	vec4 viewportToEye(const vec4& pt) const { return ndcToEye(viewportToNdc(pt)); }
	vec4 viewportToClip(const vec4& pt) const { return ndcToClip(viewportToNdc(pt)); }
	vec4 viewportToNdc(const vec4& pt) const { return vnMat_ * pt; }
	vec4 viewportToScreen(const vec4& pt) const { return vsMat_ * pt; }

	vec4 screenToLocal(const vec4& pt) const { return ndcToLocal(screenToNdc(pt)); }
	vec4 screenToWorld(const vec4& pt) const { return ndcToWorld(screenToNdc(pt)); }
	vec4 screenToEye(const vec4& pt) const { return ndcToEye(screenToNdc(pt)); }
	vec4 screenToClip(const vec4& pt) const { return ndcToClip(screenToNdc(pt)); }
	vec4 screenToNdc(const vec4& pt) const { return getNsMatR_() * pt; }
	vec4 screenToViewport(const vec4& pt) const { return getVsMatR_() * pt; }

	// 基于NDC的坐标轴翻转实现，仅对2d有效，
	// bool axisInversed(int dim) const { return invAxis_[dim]; }
	// void setAxisInversed(int dim, bool inv = true);

private:

	void resetModelRelatedMats_();

	const mat4& getMMatR_() const {
		if (!mMatR_)
			mMatR_ = localMatStack_.back().getInverse();
		return mMatR_.value();
	}

	const mat4& getMvMatR_() const {
		if (localMatStack_.empty())
			return getViewMatR_();

		if (!mvMatR_)
			mvMatR_ = getMvMat().getInverse();
		return mvMatR_.value();
	}

	const mat4& getMvpMatR_() const {
		if (localMatStack_.empty())
			return getVpMatR_();

		if (!mvpMatR_)
			mvpMatR_ = getMvpMat().getInverse();
		return mvpMatR_.value();
	}

	const mat4& getVpMatR_() const {
		if (!vpMatR_)
			vpMatR_ = vpMat_.getInverse();
		return vpMatR_.value();
	}

	const mat4& getViewMatR_() const {
		if (!viewMatR_)
			viewMatR_ = viewMat_.getInverse();
		return viewMatR_.value();
	}

	const mat4& getProjMatR_() const {
		if (!projMatR_)
			projMatR_ = projMat_.getInverse();
		return projMatR_.value();
	}

	const mat4& getVsMatR_() const {
		if (!vsMatR_)
			vsMatR_ = vsMat_.getInverse();
		return vsMatR_.value();
	}

	const mat4& getNsMatR_() const {
		if (!nsMatR_)
			nsMatR_ = nsMat_.getInverse();
		return nsMatR_.value();
	}


private:

	// viewport 
	rect vp_{ point2(0, 0), point2(1, 1) };

	// 模型矩阵栈，用于从物体局部坐标变换到全局世界坐标
	std::vector<mat4> localMatStack_;

	// view matrix
	mat4 viewMat_{ mat4::identity() };

	// standard projection matrix
	mat4 projMat_{ mat4::identity() };


    // 临时矩阵变量

	mat4 vpMat_; // proj * view
	mutable std::optional<mat4> mvMat_; // view * model
	mutable std::optional<mat4> mvpMat_; // proj * view * model
	mutable std::optional<mat4> mMatR_; // (model)-1
	mutable std::optional<mat4> viewMatR_; // (view)-1
	mutable std::optional<mat4> projMatR_; // (proj)-1
	mutable std::optional<mat4> mvMatR_; // (view * model)-1
	mutable std::optional<mat4> mvpMatR_; // (proj * view * model)-1
	mutable std::optional<mat4> vpMatR_; // (proj * view)-1

	mat4 nvMat_{
		0.5f, 0, 0, 0.5f,
		0, 0.5f, 0, 0.5f,
		0, 0, 0.5f, 0.5f,
		0, 0, 0, 1
	}; // 从ndc到viewport的转换矩阵

	mat4 vnMat_{
		2, 0, 0, -1,
		0, 2, 0, -1,
		0, 0, 2, -1,
		0, 0, 0, 1
	}; // 从viewport到ndc的变换矩阵

	mat4 vsMat_{ mat4::identity() }; // 从viewport到screen的转换矩阵
	mat4 nsMat_{ mat4::identity() }; // 从ndc到screen的转换矩阵
	mutable std::optional<mat4> vsMatR_; 
	mutable std::optional<mat4> nsMatR_; 

	// 定义viewport的水平和垂直方向
	bool invVpX_{ false }; // false表示从左至右为正向
	bool invVpY_{ true }; // true表示从上至下为正向，兼容显示屏

	mat4 wsMat_; // 从world到screen的转换矩阵，进一步将二次矩阵运算压减到一次

	// bool invAxis_[3]{ false, false, false }; // 各坐标轴的翻转状态
};


template<typename KREAL, bool ROW_MAJOR> const typename KtProjector<KREAL, ROW_MAJOR>::mat4&
KtProjector<KREAL, ROW_MAJOR>::getMvMat() const
{
	if (localMatStack_.empty())
		return viewMat_;

	if (!mvMat_)
		mvMat_ = viewMat_ * localMatStack_.back();
	return mvMat_.value();
}


template<typename KREAL, bool ROW_MAJOR> const typename KtProjector<KREAL, ROW_MAJOR>::mat4&
KtProjector<KREAL, ROW_MAJOR>::getMvpMat() const {
	if (localMatStack_.empty())
		return vpMat_;

	if (!mvpMat_)
		mvpMat_ = vpMat_ * localMatStack_.back();
	return mvpMat_.value();
}


template<typename KREAL, bool ROW_MAJOR> typename KtProjector<KREAL, ROW_MAJOR>::mat4
KtProjector<KREAL, ROW_MAJOR>::getNormalMatrix() const
{
	return getMvMat().getInverse().getTranspose();
}


template<typename KREAL, bool ROW_MAJOR>
void KtProjector<KREAL, ROW_MAJOR>::setViewport(const rect& vp)
{
	vp_ = vp;

	if (!invVpX_) {
		vsMat_.m00() = vp.width();
		vsMat_.m03() = vp.lower().x();
	}
	else {
		vsMat_.m00() = -vp.width();
		vsMat_.m03() = vp.lower().x() + vp.width();
	}

	if (invVpY_) {
		vsMat_.m11() = -vp.height();
		vsMat_.m13() = vp.lower().y() + vp.height();
	}
	else {
		vsMat_.m11() = vp.height();
		vsMat_.m13() = vp.lower().y();
	}

	// x, y方向各偏移0.5，相当于作round
	//vsMat_.m03() += 0.5;
	//vsMat_.m13() += 0.5;

	nsMat_ = vsMat_ * nvMat_;
	wsMat_ = nsMat_ * vpMat_;

	vsMatR_.reset();
	nsMatR_.reset();
}


template<typename KREAL, bool ROW_MAJOR>
void KtProjector<KREAL, ROW_MAJOR>::updateProjectMatrixs()
{
	vpMat_ = projMat_ * viewMat_;
	wsMat_ = nsMat_ * vpMat_;

	vpMatR_.reset();
	viewMatR_.reset();
	projMatR_.reset();

	resetModelRelatedMats_();
}


template<typename KREAL, bool ROW_MAJOR>
void KtProjector<KREAL, ROW_MAJOR>::resetModelRelatedMats_()
{
	mvMat_.reset();
	mvpMat_.reset();
	mMatR_.reset();
	mvMatR_.reset();
	mvpMatR_.reset();
}


template<typename KREAL, bool ROW_MAJOR>
KtVector3<KREAL> KtProjector<KREAL, ROW_MAJOR>::getEyePos() const
{
	return viewMat_.getTranslation();
}


template<typename KREAL, bool ROW_MAJOR>
KtQuaternion<KREAL> KtProjector<KREAL, ROW_MAJOR>::getEyeOrient() const
{
	return viewMat_.getRotation(); // TODO:
}

#if 0
template<typename KREAL, bool ROW_MAJOR>
void KtProjector<KREAL, ROW_MAJOR>::setAxisInversed(int dim, bool inv)
{
	if (axisInversed(dim) == inv)
		return; // 状态一致，直接返回

	invAxis_[dim] = inv;

	nvMat_.data()[dim * 5] *= -1; // dim * 5 = dim * 4 + dim = [dim][dim]
	vnMat_.data()[dim * 5] *= -1;

	nsMat_ = vsMat_ * nvMat_;
	wsMat_ = nsMat_ * vpMat_;
}
#endif