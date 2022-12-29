#include "KcCoord2d.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"
#include "layout/KuLayoutHelper.h"


KcCoord2d::KcCoord2d()
    : KcCoord2d(point2(0), point2(1))
{
	
}


KcCoord2d::KcCoord2d(const point2& lower, const point2& upper)
	: KvCoord("CoordSystem2d")
{
	int dim[4];
	dim[KcAxis::k_top] = dim[KcAxis::k_bottom] = 0;
	dim[KcAxis::k_left] = dim[KcAxis::k_right] = 1;

	// ��ʼ��4����������
	for (unsigned i = 0; i < 4; i++) 
		axes_[i].emplace_back(new KcAxis(KcAxis::KeType(i), dim[i], true));

	setExtents({ lower.x(), lower.y(), -1 }, { upper.x(), upper.y(), 1 });

	axes_[KcAxis::k_left].front()->tickOrient() =
		axes_[KcAxis::k_left].front()->labelOrient() = -KcAxis::vec3::unitX();
	axes_[KcAxis::k_right].front()->tickOrient() =
		axes_[KcAxis::k_right].front()->labelOrient() = KcAxis::vec3::unitX();
	axes_[KcAxis::k_bottom].front()->tickOrient() =
		axes_[KcAxis::k_bottom].front()->labelOrient() = -KcAxis::vec3::unitY();
	axes_[KcAxis::k_top].front()->tickOrient() =
		axes_[KcAxis::k_top].front()->labelOrient() = KcAxis::vec3::unitY();

	static const char* title[] = { "X", "Y" };
	for (unsigned i = 0; i < 4; i++)
		axes_[i].front()->title() = title[axes_[i].front()->dim()];

	axes_[KcAxis::k_right].front()->visible() = false;
	axes_[KcAxis::k_top].front()->visible() = false;

	// FIXME: Ϊ�˼���3d���˴�ʹ��k_back���ͣ�ʵ��ӦΪk_front
	plane_ = std::make_shared<KcCoordPlane>(KcCoordPlane::k_back, axes_[KcAxis::k_left].front(),
		axes_[KcAxis::k_right].front(),
		axes_[KcAxis::k_bottom].front(),
		axes_[KcAxis::k_top].front());

	putAt(0, 0, plane_.get());
}


KcCoord2d::~KcCoord2d()
{
	// ���layout�е�plane��axis���󣬷�ֹ����release
	forPlane([this](KcCoordPlane& plan) {
		KuLayoutHelper::take(&plan);
		return true; });
	forAxis([this](KcAxis& axis) {
		KuLayoutHelper::take(&axis);
		return true; });
}


void KcCoord2d::forAxis(std::function<bool(KcAxis& axis)> fn) const
{
	for (unsigned i = 0; i < std::size(axes_); i++)
		for(auto& aixs : axes_[i])
		    if(!fn(*aixs))
				return;
}


void KcCoord2d::forPlane(std::function<bool(KcCoordPlane& plane)> fn) const
{
	fn(*plane_);
}


KtMargins<KcCoord2d::float_t> KcCoord2d::calcMargins_(KvPaint* paint) const
{
	auto l = axes_[KcAxis::k_left].front()->calcMargins(paint);
	auto r = axes_[KcAxis::k_right].front()->calcMargins(paint);
	auto t = axes_[KcAxis::k_top].front()->calcMargins(paint);
	auto b = axes_[KcAxis::k_bottom].front()->calcMargins(paint);

	l.makeCeil(r); l.makeCeil(b); l.makeCeil(t);

	return l;
}


KcCoord2d::size_t KcCoord2d::calcSize_(void* cxt) const
{
	// ���²���axis
	forAxis([this](KcAxis& axis) {
		KuLayoutHelper::take(&axis);
		return true; });
	
	forAxis([this](KcAxis& axis) {
		if (axis.visible() && axis.length() > 0) {
			switch (axis.typeReal())
			{
			case KcAxis::k_left:
				KuLayoutHelper::placeLeft(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_right:
				KuLayoutHelper::placeRight(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_top:
				KuLayoutHelper::placeTop(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			case KcAxis::k_bottom:
				KuLayoutHelper::placeBottom(plane_.get(), &axis, axis.main() ? 0 : -1);
				break;

			default:
				break;
			}
		}
		return true; });

	return __super::calcSize_(cxt);
}


KcCoord2d::rect_t KcCoord2d::getPlotRect() const
{
	return plane_->innerRect();
}


void KcCoord2d::placeElement(KvLayoutElement* ele, KeAlignment loc)
{
	assert(!isAncestorOf(ele));

	if (loc.inner()) {
		plane_->insert(ele);
	}
	else {
		if (loc & KeAlignment::k_horz_first) {
			if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(plane_.get(), ele, -1);
			else
				assert(false);
		}
		else {
			if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(plane_.get(), ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(plane_.get(), ele, -1);
			else
				assert(false);
		}
	}
}
