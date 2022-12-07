#pragma once
#include <functional>
#include "KvRenderable.h"
#include "KtMatrix4.h"
#include "layout/KcLayoutGrid.h"

class KcAxis; 
class KcCoordPlane;

// ��ͼ����ϵ�ĳ���ӿ�
// ��������ϵ���š����ƺ������ᷴת��������ʵ��

class KvCoord : public KvRenderable, public KcLayoutGrid
{
public:
	using float_t = typename KvRenderable::float_t;
	using point3 = KtPoint<float_t, 3>;
	using mat4 = KtMatrix4<float_t>;

	using KvRenderable::KvRenderable;

	virtual ~KvCoord() {}

	virtual void setExtents(const point3& lower, const point3& upper) = 0;;

	// the lower conner
	virtual point3 lower() const = 0; 

	// the upper conner
	virtual point3 upper() const = 0; 

	// ��ѯ������
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// ��ѯ����ƽ��
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	// ����ʵ�ʵĻ�ͼ����paint��������Ϊviewport��֮�����plottables
	virtual rect_t getPlotRect() const = 0;

	virtual void placeElement(KvLayoutElement* ele, KeAlignment loc) = 0;

	// the center point
	point3 center() const {
		return (upper() + lower()) / 2;
	}

	// ������ϵAABB�����ĵ�Ϊ��׼���Ը��������extent/range���еȱ�������
	// ������ϵ����ǰ��AABB�����ĵ㱣�ֲ���
	// factor=1ʱ������ϵ������
	// factor=0ʱ������ϵ���������ĵ�
	void zoom(float_t factor);

	void inverseAxis(int dim, bool inv); // ��תdimά�ȵ���������
	bool axisInversed(int dim) const; // dimά���������Ƿ�ת
	bool axisInversed() const; // �����������ᷴת���򷵻�true

	enum KeAxisSwapStatus
	{
		k_axis_swap_none,
		k_axis_swap_xy,
		k_axis_swap_xz,
		k_axis_swap_yz
	};

	void swapAxis(KeAxisSwapStatus status);
	KeAxisSwapStatus axisSwapped() const { return swapStatus_; }

	// ���������ᷴת�ͽ��������ɵı任���󣬿����ڻ���plottable
	mat4 localMatrix() const;

	// �ṩ����ӿڵ�ȱʡʵ��

	void draw(KvPaint*) const override;

	aabb_t boundingBox() const override;

private:
	KeAxisSwapStatus swapStatus_{ k_axis_swap_none }; // ���������ύ����״̬
};
