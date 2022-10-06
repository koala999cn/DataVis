#pragma once
#include "KtVector3.h"
#include "KtCamera.h"


template<typename REAL, bool ROW_MAJOR = true>
class KtTrackballController
{
	using vec3 = KtVector3<REAL>;
    using vec4 = KtVector4<REAL>;
    using camera_type = KtCamera<REAL, ROW_MAJOR>;

public:

    KtTrackballController(camera_type& cam) : cam_(cam) {}


	// 设置起始位置，开始新一轮操纵
	// @x, y: 当前鼠标移动位置(屏幕坐标)
	void reset(REAL x, REAL y);

	// 操纵Trackball
	void steer(REAL x, REAL y);

private:

	// 将2d屏幕坐标变换到Trackball表面的3d世界坐标
	vec3 project_(REAL x, REAL y) const;

private:
    camera_type& cam_;
};
