#include "KcBars3d.h"
#include "KuPrimitiveFactory.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include "KvData.h"


void KcBars3d::setBarWidthRatioZ(float w)
{
	barWidthRatioZ_ = w;
	setDataChanged(false);
}


KcBars3d::aabb_t KcBars3d::calcBoundingBox_() const
{
	auto box = super_::calcBoundingBox_();

	// ÐÞÕýzÖá
	auto zw = barWidth_(zdim());
	box.lower().z() -= zw / 2; box.upper().z() += zw / 2;

	return box;
}


std::pair<unsigned, unsigned> KcBars3d::vtxSizePerBar_() const
{
	return { KuPrimitiveFactory::makeBox<float>(point3f(0), point3f(0), nullptr),
			 KuPrimitiveFactory::indexBox<unsigned>(nullptr) };
}


void KcBars3d::drawOneBar_(float_t* pos, unsigned ch, float_t bottom, void* vtxBuf, void* idxBuf, unsigned idxBase) const
{
	auto xw = barWidthRatio() * barWidth_(xdim());
	auto zw = barWidthRatioZ() * barWidth_(zdim());

	auto pt0 = toPoint_(pos, ch);
	pt0.x() += xw * 0.5; pt0.z() += zw * 0.5;
	decltype(pt0) pt1{ pt0.x() - xw, bottom, pt0.z() - zw };

	struct KpVertexPC
	{
		point3f pos;
		point4f clr;
	};

	auto vtxSize = vtxSizePerBar_();
	auto vtx = (KpVertexPC*)vtxBuf;
	KuPrimitiveFactory::makeBox<float>(point3f(pt1), point3f(pt0), vtx, sizeof(KpVertexPC));
	for (unsigned i = 0; i < vtxSize.first; i++) {
		pos[xdim()] = vtx[i].pos.x();
		pos[ydim()] = vtx[i].pos.y();
		if (odata()->dim() > 1)
		    pos[zdim()] = vtx[i].pos.z();
		vtx[i].clr = mapValueToColor_(pos, ch);
	}

	KuPrimitiveFactory::indexBox<std::uint32_t>((std::uint32_t*)idxBuf);
	for (unsigned i = 0; i < vtxSize.second; i++)
		((std::uint32_t*)idxBuf)[i] += idxBase;
}
