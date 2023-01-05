#pragma once
#include "KvCoord.h"
#include <vector>
#include <memory>
#include "layout/KcLayoutOverlay.h"


// ��ά����ϵʵ�֣���12���������6��ƽ�湹��
// Ĭ��X�����ң�Y�����ϣ�Z������

class KcCoord3d : public KvCoord
{
public:
	using axis_ptr = std::shared_ptr<KcAxis>;
	using grid_plane_ptr = std::shared_ptr<KcCoordPlane>;
	

public:

	KcCoord3d();
	KcCoord3d(const point3& lower, const point3& upper);

	void forAxis(std::function<bool(KcAxis& axis)>) const override;

	void forPlane(std::function<bool(KcCoordPlane& plane)>) const override;

	rect_t getPlotRect() const override;

	void placeElement(KvLayoutElement* ele, KeAlignment loc) final;

	void arrange(const rect_t& rc) final;

	axis_ptr& axis(int type) { return axes_[type]; }
	axis_ptr axis(int type) const { return axes_[type]; }


private:
	size_t calcSize_(void* cxt) const final;

private:
	axis_ptr axes_[12];
	grid_plane_ptr planes_[6];
	std::unique_ptr<KcLayoutOverlay> layCoord_;
	mutable rect_t rcCoord_; // coord3d����Ļ����rect��������Ϊlegend��colorbar�Ļ�׼. ��calcSize_�������
};
