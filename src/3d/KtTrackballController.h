#pragma once
#include "KtVector3.h"
#include "KtQuaternion.h"


// 根据屏幕2d坐标变化操纵3d空间的方位角
// 基于四元数实现

template<typename KREAL>
class KtTrackballController
{
    using point2 = KtPoint<KREAL, 2>;
    using point3 = KtPoint<KREAL, 3>;
	using vec3 = KtVector3<KREAL>;
    using vec4 = KtVector4<KREAL>;
	using quat = KtQuaternion<KREAL>;

public:

    // 初始化一次track操作
    // @pos: 当前鼠标移动位置(屏幕坐标)
    // @pivot: 方位角变换的支点（屏幕坐标），此处假定为trackball的中心点
    // @radius: trackball在屏幕坐标x轴和y轴方向的覆盖半径
    void reset(point2 pos, point2 pivot, point2 radius) {
        pivot_ = pivot, radius_ = radius;
        lastPos_ = pos;
        lastPos3d_ = project_(lastPos_);
    }

	// 核心算法，根据新的屏幕坐标操纵Trackball，更新方位角
    quat steer(KREAL dx, KREAL dy);

private:

	// 将2d屏幕坐标变换到Trackball表面的3d世界坐标
	vec3 project_(const point2& pt) const;

private:

    // trackball的参数：支点与半径
    point2 pivot_{ 0, 0 };
    point2 radius_{ 1, 1 };
    KREAL rotateSpeed_{ 0.1 };

    point2 lastPos_; // 追踪鼠标位置
    vec3 lastPos3d_; // mousePos_在trackball的投影坐标
};


template<typename KREAL>
typename KtTrackballController<KREAL>::quat KtTrackballController<KREAL>::steer(KREAL dx, KREAL dy)
{
    auto curPos = lastPos_ + point2(dx, dy) * rotateSpeed_;
    auto curPos3d = project_(curPos);

    quat rot(lastPos3d_, curPos3d);
    lastPos_ = curPos;
    lastPos3d_ = curPos3d;
    return rot;
}


template<typename KREAL>
KtVector3<KREAL> KtTrackballController<KREAL>::project_(const point2& pt) const
{
    // 将pt归一化
    auto npt = pt - pivot_;
    npt /= radius_;
    npt.y() = -npt.y();

    // 算法关键在于求z
    // 若sqrt(x*x+y*y) <= r/sqrt(2), z = sqrt(r*r-x*x-y*y)
    // 否则, z = r*r/2/sqrt(x*x+y*y)
    // 在归一化情况下，r=1
    KREAL len2 = npt.squaredLength();
    KREAL z = len2 <= 0.5 ? std::sqrt(1 - len2) : 0.5 / std::sqrt(len2);
    return vec3(npt.x(), npt.y(), z).getNormalize();
}

