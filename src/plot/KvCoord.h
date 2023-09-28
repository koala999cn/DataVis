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

	void setExtents(const point3& lower, const point3& upper);

	// the lower conner
	point3 lower() const { return extent_[0]; }

	// the upper conner
	point3 upper() const { return extent_[1]; }

	// ��ѯ������
	virtual void forAxis(std::function<bool(KcAxis& axis)>) const = 0;

	// ��ѯ����ƽ��
	virtual void forPlane(std::function<bool(KcCoordPlane& plane)>) const = 0;

	// ����ʵ�ʵĻ�ͼ����paint��������Ϊviewport��֮�����plottables
	virtual rect_t getPlotRect() const = 0;

	virtual void placeElement(KvLayoutElement* ele, KeAlignment loc) = 0;

	// ����dimά�ȵ�ȱʡ�������ᣬ��������plottable��axis
	virtual std::shared_ptr<KcAxis> defaultAxis(unsigned dim) const = 0;

	// the center point
	point3 center() const {
		return (upper() + lower()) / 2;
	}

	// ������ϵAABB�����ĵ�Ϊ��׼���Ը��������extent/range���еȱ�������
	// ������ϵ����ǰ��AABB�����ĵ㱣�ֲ���
	// factor=1ʱ������ϵ������
	// factor=0ʱ������ϵ���������ĵ�
	void zoom(float_t factor);

	/// ����3����Ա����ֻ�������������в���

	// ��תdimά�ȵ���������
	void inverseAxis(int dim, bool inv); 

	// dimά���������Ƿ�ת
	bool axisInversed(int dim) const { return inv_[dim]; }

	// �����������ᷴת���򷵻�true
	bool axisInversed() const {
		return axisInversed(0) || axisInversed(1) || axisInversed(2);
	}

	enum KeAxisSwapStatus
	{
		k_axis_swap_none,
		k_axis_swap_xy,
		k_axis_swap_xz,
		k_axis_swap_yz
	};

	void swapAxis(KeAxisSwapStatus status);
	KeAxisSwapStatus axisSwapped() const { return swapStatus_; }

	mat4 axisInverseMatrix() const;

	const mat4& axisSwapMatrix() const;

	// ���������ᷴת�ͽ��������ɵı任���󣬿����ڻ���plottable
	// mat4 localMatrix() const;

	// �ṩ����ӿڵ�ȱʡʵ��

	void draw(KvPaint*) const override;

	// ���ؾֲ������aabb
	aabb_t boundingBox() const override;

private:

	mat4 axisReflectMatrix_(int dim) const;

	void resetAxisExtent_(KcAxis& axis, bool swap) const;

private:
	point3 extent_[2]; // ��������ķ�Χ��extent_[0] = lower, extent_[1] = upper
	bool inv_[3]{ false }; // �����������ᣨx/y/z���ķ�ת״̬�����ڿ��ٷ���
	KeAxisSwapStatus swapStatus_{ k_axis_swap_none }; // ���������ύ����״̬
};
