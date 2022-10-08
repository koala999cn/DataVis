#pragma once
#include "KtVector3.h"
#include "KtQuaternion.h"


// ������Ļ2d����仯����3d�ռ�ķ�λ��
// ������Ԫ��ʵ��

template<typename REAL>
class KtTrackballController
{
    using point2 = KtPoint<REAL, 2>;
    using point3 = KtPoint<REAL, 3>;
	using vec3 = KtVector3<REAL>;
    using vec4 = KtVector4<REAL>;
	using quat = KtQuaternion<REAL>;

public:

	KtTrackballController(quat& orient) : orient_(orient) {}

    // ��ʼ��һ��track����
    // @pos: ��ǰ����ƶ�λ��(��Ļ����)
    // @pivot: ��λ�Ǳ任��֧�㣨��Ļ���꣩���˴��ٶ�Ϊtrackball�����ĵ�
    // @radius: trackball����Ļ����x���y�᷽��ĸ��ǰ뾶
    void reset(point2 pos, point2 pivot, point2 radius) {
        pivot_ = pivot, radius_ = radius;
        lastPos_ = pos;
        lastPos3d_ = project_(lastPos_);
        orient_.normalize(); // �����ۼ����
    }

	// �����㷨�������µ���Ļ�������Trackball�����·�λ��
	void steer(REAL dx, REAL dy);

private:

	// ��2d��Ļ����任��Trackball�����3d��������
	vec3 project_(const point2& pt) const;

private:
	quat& orient_; // ���ٿصķ�λ��

    // trackball�Ĳ�����֧����뾶
    point2 pivot_{ 0, 0 };
    point2 radius_{ 1, 1 };
    REAL rotateSpeed_{ 0.1 };

    point2 lastPos_; // ׷�����λ��
    vec3 lastPos3d_; // mousePos_��trackball��ͶӰ����
};


template<typename REAL>
void KtTrackballController<REAL>::steer(REAL dx, REAL dy)
{
    auto curPos = lastPos_ + point2(dx, dy) * rotateSpeed_;
    auto curPos3d = project_(curPos);

    quat rot(lastPos3d_, curPos3d);
    lastPos_ = curPos;
    lastPos3d_ = curPos3d;
    orient_ = rot * orient_;
}


template<typename REAL>
KtVector3<REAL> KtTrackballController<REAL>::project_(const point2& pt) const
{
    // ��pt��һ��
    auto npt = pt - pivot_;
    npt /= radius_;
    npt.y() = -npt.y();

    // �㷨�ؼ�������z
    // ��sqrt(x*x+y*y) <= r/sqrt(2), z = sqrt(r*r-x*x-y*y)
    // ����, z = r*r/2/sqrt(x*x+y*y)
    // �ڹ�һ������£�r=1
    REAL len2 = npt.squaredLength();
    REAL z = len2 <= 0.5 ? std::sqrt(1 - len2) : 0.5 / std::sqrt(len2);
    return vec3(npt.x(), npt.y(), z).getNormalize();
}

