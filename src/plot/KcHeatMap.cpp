#include "KcHeatMap.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KuPrimitiveFactory.h"
#include "KvDiscreted.h"
#include <sstream>


KcHeatMap::KcHeatMap(const std::string_view& name)
	: super_(name)
{
	setForceDefaultZ(true); // ��3d�ռ���ʾ��ͼƽ��
	setFlatShading(true);
	setColoringMode(k_colorbar_gradiant);
}


void KcHeatMap::setData(const_data_ptr d)
{
	super_::setData(d);
	setLabelingDim(odim());
}


unsigned KcHeatMap::objectCount() const
{
	return empty() ? 0 : channels_() + 1/*label*/;
}


bool KcHeatMap::objectVisible_(unsigned objIdx) const
{
	if (objIdx == channels_()) 
		return showLabel();
	else
		return super_::objectVisible_(objIdx);
}



bool KcHeatMap::objectReusable_(unsigned objIdx) const
{
	if (objIdx == channels_()) {
		return !dataChanged() && !labelChanged();
	}
	else {
		return super_::objectReusable_(objIdx);
	}
}


void KcHeatMap::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx == channels_()) {
		paint->apply(label().font);
		paint->setColor(label().color);
	}
	else
		super_::setObjectState_(paint, objIdx);
}


std::pair<KcHeatMap::float_t, KcHeatMap::float_t> KcHeatMap::xyshift_() const
{
	auto disc = discreted_();
	auto dx = (xdim() == odim()) ? 1 : std::abs(disc->step(xdim()));
	auto dy = (ydim() == odim()) ? 1 : std::abs(disc->step(ydim()));
	assert(KuMath::isDefined(dx) && dx != 0);
	assert(KuMath::isDefined(dy) && dy != 0);

	return { dx / 2, dy / 2 };
}


KcHeatMap::aabb_t KcHeatMap::calcBoundingBox_() const
{
	// TODO���ݲ�֧��ά��ӳ��
	//const_cast<KcHeatMap*>(this)->setXdim(odim() - 2);
	//const_cast<KcHeatMap*>(this)->setYdim(odim() - 1);
	//const_cast<KcHeatMap*>(this)->setZdim(odim());

	auto aabb = super_::calcBoundingBox_();

	if (!empty() && odata()->dim() > 1) {
		auto xyshift = xyshift_();
		aabb.inflate(xyshift.first, xyshift.second);
	}

	return aabb;
}


void* KcHeatMap::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	// TODO���ݲ�֧��ά��ӳ��
	//const_cast<KcHeatMap*>(this)->setXdim(odim() - 2);
	//const_cast<KcHeatMap*>(this)->setYdim(odim() - 1);
	//const_cast<KcHeatMap*>(this)->setZdim(odim());

	if (objIdx == channels_())
		return drawLabel_(paint);
	else 
		return drawGrid_(paint, objIdx);
}


void* KcHeatMap::drawGrid_(KvPaint* paint, unsigned ch) const
{
	auto xyshift = xyshift_();

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	// ��ͼ���ݵ���У�����grid��һ�ж�һ��
	auto nx = linesPerGrid_() + 1; // ����
	auto ny = sizePerLine_() + 1; // ����
	auto grids = gridsPerChannel_();

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_>>(k_quads);
	auto idxPerGrid = KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, ny, nullptr);
	geom->reserve(nx * ny * gridsTotal_(), idxPerGrid * gridsTotal_());
	std::uint32_t idxBase(0);

	auto shape = KuDataUtil::shape(*discreted_());

	for (unsigned i = 0; i < grids; i++) {
		for (unsigned j = 0; j < nx; j++) {
			auto line = gridLineAt_(ch, i, j > 0 ? j - 1 : 0);
			assert(line.size == ny - 1);

			auto vtx = geom->newVertex(ny);

			for (unsigned k = 0; k < ny; k++) {
				auto pt = line.getter(k > 0 ? k - 1 : 0);

				if (j > 0)
					pt[xdim()] += xyshift.first;
				else 
					pt[xdim()] -= xyshift.first; // ��������Ϊ�����һ��

				if (k > 0)
					pt[ydim()] += xyshift.second;
				else 
					pt[ydim()] -= xyshift.second; // ��������Ϊ�����һ��

				vtx->pos = toPoint_(pt.data(), ch);
				vtx->clr = mapValueToColor_(pt.data(), ch);
				++vtx;
			}
		}

		auto idx = geom->newIndex(idxPerGrid);

		// ����grid���������Ϸ�ƫ�ƣ�����Ӧ��֤quad�����һ�����������Ͻǣ�openglĬ��falt��Ⱦģʽʹ�����һ����������ݣ�
		// ����趨startVtxΪ3�������Ϊ���½ǣ����˳ʱ����ת���ɱ�֤���Ͻ�Ϊ�յ�
		KuPrimitiveFactory::indexGrid<>(ny, nx, idx, 3, idxBase); // ���ڶ��㰴ny��˳������д��ģ����Դ˴�ny��ǰ
		idxBase += nx * ny;
	}

	return paint->drawGeomColor(geom);
}


void* KcHeatMap::drawLabel_(KvPaint* paint) const
{
	auto xyshift = xyshift_();

	auto leng = paint->projectv({ xyshift.first * 2, xyshift.second * 2, 0 }).abs();
	auto minSize = paint->textSize("0");
	if (leng.x() < minSize.x() || leng.y() < minSize.y()) // ��1�������жϣ�����nx*ny�ܴ�ʱ���ǳ���ʱ
		return nullptr;

	std::vector<point3> anchors; anchors.reserve(gridsTotal_());
	std::vector<std::string> texts; texts.reserve(gridsTotal_());
	
	std::ostringstream strm;
	label().formatStream(strm);

	auto nx = sizePerLine_();
	auto ny = linesPerGrid_();
	auto grids = gridsPerChannel_();
	for (unsigned ch = 0; ch < channels_(); ch++) {
		for (unsigned i = 0; i < grids; i++) {
			for (unsigned j = 0; j < ny; j++) {
				auto line = gridLineAt_(ch, i, j);
				assert(line.size == nx);
				for (unsigned k = 0; k < nx; k++) {
					auto pt = line.getter(k);
					anchors.push_back(toPoint_(pt.data(), ch));
					strm.str("");
					strm << pt[labelingDim()];
					texts.push_back(strm.str());
				}
			}
		}
	}

	const_cast<KcHeatMap*>(this)->labelChanged() = false;
	return paint->drawTexts(anchors, texts, label().align, label().spacing);
}
