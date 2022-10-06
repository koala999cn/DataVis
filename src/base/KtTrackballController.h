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


	// ������ʼλ�ã���ʼ��һ�ֲ���
	// @x, y: ��ǰ����ƶ�λ��(��Ļ����)
	void reset(REAL x, REAL y);

	// ����Trackball
	void steer(REAL x, REAL y);

private:

	// ��2d��Ļ����任��Trackball�����3d��������
	vec3 project_(REAL x, REAL y) const;

private:
    camera_type& cam_;
};
