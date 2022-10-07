#pragma once
#include "KtVector3.h"
#include "KtQuaternion.h"


// ������Ļ2d����仯����3d�ռ�ķ�λ��
// ������Ԫ��ʵ��

template<typename REAL>
class KtTrackballController
{
    using point2 = KtPoint<REAL, 2>;
	using vec3 = KtVector3<REAL>;
    using vec4 = KtVector4<REAL>;
	using quat = KtQuaternion<REAL>;

public:

	KtTrackballController(quat& orient) : orient_(orient) {}

    // ����trackball
    // @pivot: ��λ�Ǳ任��֧�㣨��Ļ���꣩���˴��ٶ�Ϊtrackball�����ĵ�
    // @radius: trackball����Ļ����x���y�᷽��ĸ��ǰ뾶
    void reset(point2 pivot, point2 radius) {
        pivot_ = pivot, radius_ = radius;
    }

    // ��ʼ��һ��track����
    // @x, y: ��ǰ����ƶ�λ��(2d��Ļ����)
    void start(REAL x, REAL y) {
        mousePos_ = { x, y };
        ballPos_ = project_(mousePos_);
        orient_.normalize(); // �����ۼ����
    }

	// �����㷨�������µ���Ļ�������Trackball�����·�λ��
	void delta(REAL dx, REAL dy);

private:

	// ��2d��Ļ����任��Trackball�����3d��������
	vec3 project_(const point2& pt) const;

private:
	quat& orient_; // ���ٿصķ�λ��

    // trackball�Ĳ�����֧����뾶
    point2 pivot_, radius_; 

    point2 mousePos_; // ׷�����λ��
    vec3 ballPos_; // mousePos_��trackball��ͶӰ����
};


template<typename REAL>
void KtTrackballController<REAL>::delta(REAL dx, REAL dy)
{
    mousePos_ += point2(dx, dy);
    auto v = project_(mousePos_);

    quat rot(ballPos_, v);
    ballPos_ = v;
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
    REAL xy2 = npt.x() * npt.x() + npt.y() * npt.y();
    REAL z = xy2 <= 0.5 ? std::sqrt(1 - xy2) : 0.5 / std::sqrt(xy2);
    return vec3(npt.x(), npt.y(), z).getNormalized();
}

