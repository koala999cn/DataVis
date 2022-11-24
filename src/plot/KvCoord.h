#pragma once
#include <functional>
#include "KvRenderable.h"
#include "KtMargins.h"

class KcAxis; 
class KcCoordPlane;

// ��ͼ����ϵ�ĳ���ӿ�

class KvCoord : public KvRenderable
{
public:
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	virtual ~KvCoord() {}

	virtual void setExtents(const point3& lower, const point3& upper) = 0;;

	// the lower conner
	virtual point3 lower() const = 0; 

	// the upper conner
	virtual point3 upper() const = 0; 

	// the center point
	point3 center() const {
		return (upper() + lower()) / 2;
	}

	// ������ϵAABB�����ĵ�Ϊ��׼���Ը��������extent/range���еȱ�������
	// ������ϵ����ǰ��AABB�����ĵ㱣�ֲ���
	// factor=1ʱ������ϵ������
	// factor=0ʱ������ϵ���������ĵ�
	void zoom(float_t factor);

	// ��ѯ������
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// ��ѯ����ƽ��
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	virtual KtMargins<float_t> calcMargins(KvPaint*) const = 0;

	// �ṩ����ӿڵ�ȱʡʵ��

	void draw(KvPaint*) const override;

	aabb_type boundingBox() const override;
};
