#include "KvPaint.h"
#include "3d/KcVertexDeclaration.h"
#include "layout/KuLayoutUtil.h"
#include "KuPrimitiveFactory.h"


void* KvPaint::drawMarkers(point_getter fn, unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawMarker(fn(i));

	return nullptr;
}


void* KvPaint::drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count)
{
	for (unsigned i = 0; i < count; i++) {
		if (clr) setColor(clr(i));
		if (size) setMarkerSize(size(i));
		drawMarker(fn(i));
	}

	return nullptr;
}


void* KvPaint::drawLineStrip(point_getter fn, unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(fn(i - 1), fn(i)); // TODO: 待优化，1个point2次调用fn

	return nullptr;
}


void* KvPaint::drawLineLoop(point_getter fn, unsigned count) 
{
	drawLineStrip(fn, count);
	if (count > 2)
		drawLine(fn(count - 1), fn(0));

	return nullptr;
}


void KvPaint::drawRect(const point3& lower, const point3& upper)
{
	point3 pts[4];
	pts[0] = lower, pts[2] = upper;
	pts[1] = { lower.x(), upper.y(), lower.z() };
	pts[3] = { upper.x(), lower.y(), upper.z() };

	drawLineLoop(pts, 4);
}


void KvPaint::drawBox(const point3& lower, const point3& upper)
{
	// TODO: 
	drawRect(lower, upper);
}


void KvPaint::fillQuad(point3 pts[4])
{
	fillTriangle(pts);
	std::swap(pts[0], pts[1]);
	fillTriangle(pts + 1);
}


void KvPaint::fillQuad(point3 pts[4], color_t clrs[4])
{
	fillTriangle(pts, clrs);
	std::swap(pts[0], pts[1]); std::swap(clrs[0], clrs[1]);
	fillTriangle(pts + 1, clrs + 1);
}


void KvPaint::drawText(const point3& anchor, const std::string_view& text, int align, const point2f& spacing)
{
	auto szText = textSize(text);
	auto an = projectp(anchor); // 变换到屏幕坐标计算布局
	auto r = KuLayoutUtil::anchorAlignedRect({ an.x(), an.y() }, szText, align);
	auto topLeft = unprojectp(point2(r.lower().x() + spacing.x(), r.lower().y() + spacing.y()));
	topLeft.z() = anchor.z();
	drawText(topLeft, unprojectv(point3(1, 0, 0)), unprojectv(point3(0, 1, 0)), text);
}


void* KvPaint::drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing)
{
	assert(anchors.size() == texts.size());
	for (unsigned i = 0; i < anchors.size(); i++)
		drawText(anchors[i], texts[i], align, spacing);
	return nullptr;
}


void* KvPaint::drawGeomSolid(geom_ptr geom)
{
	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	return drawGeom(decl, geom);
}


void* KvPaint::drawGeomColor(geom_ptr geom)
{
	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);
	return drawGeom(decl, geom);
}


void KvPaint::apply(const KpPen& cxt)
{
	setColor(cxt.color);
	setLineWidth(cxt.width);
	setLineStyle(cxt.style);
}


void KvPaint::apply(const KpBrush& cxt)
{
	setColor(cxt.color);
}


void KvPaint::apply(const KpFont& cxt)
{
	// TODO:
}


void KvPaint::apply(const KpMarker& cxt)
{
	setMarkerType(cxt.type);
	setMarkerSize(cxt.size);
	setLineWidth(cxt.weight);
	setLineStyle(KpPen::k_solid);
	setColor(cxt.fill); // NB: fill始终是主色，当marker可填充时，fill为填充色，否则fill为线条色
	setFilled(cxt.showFill);
	setEdged(cxt.showOutline);
	if (cxt.hasOutline())
		setSecondaryColor(cxt.outline);
}


void KvPaint::drawMarker(const point3& pt)
{
	auto type = markerType();
	if (type == KpMarker::k_dot)
		type = KpMarker::k_circle;

	auto scale = unprojectv({ markerSize(), markerSize() });

	auto vtx = KuPrimitiveFactory::marker<float_t>(type);
	std::vector<point3> pts(vtx.second);
	for (unsigned i = 0; i < vtx.second; i++)
		pts[i] = { scale.x() * vtx.first[i][0] + pt.x(),
				   scale.y() * vtx.first[i][1] + pt.y(),
		           0 };

	switch (type)
	{
	case KpMarker::k_cross:
	case KpMarker::k_asterisk:
	case KpMarker::k_plus:
		for (unsigned i = 0; i < vtx.second / 2; i++)
			drawLine(pts[i * 2], pts[i * 2 + 1]);
		break;

	case KpMarker::k_circle:
		// fall through

	case KpMarker::k_left:
	case KpMarker::k_up:
	case KpMarker::k_down:
	case KpMarker::k_right:
	case KpMarker::k_square:
	case KpMarker::k_diamond:
		if (filled())
			fillPoly(pts.data(), vtx.second);
		if (edged()) {
			auto clr = color();
			if (filled())
				setColor(secondaryColor());
			drawLineLoop(pts.data(), vtx.second); // TODO: 引入path，如此可stroke和fill
			if (filled())
				setColor(clr);
		}
		break;

	default:
		break;
	}
}
