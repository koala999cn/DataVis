#include "KcMarkerObject.h"
#include "glad.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "KcVertexDeclaration.h"
#include "KuPrimitiveFactory.h"
#include "KcGpuBuffer.h"
#include "KtLine.h"


KcMarkerObject::KcMarkerObject()
    : super_(k_points) // 初始类型无关紧要，后续将根据marker参数重置
{
	marker_.type = -1;
}


void KcMarkerObject::draw() const
{
	if (type_ == k_points) {
		glPointSize(marker_.size);
	}
	else {
		prog_->useProgram();
		auto loc = prog_->getUniformLocation("vScale");
		assert(loc != -1);
		glUniform2f(loc, marker_.size * scale_.x(), marker_.size * scale_.y());
	}

    super_::draw();
}


KcRenderObject* KcMarkerObject::clone() const
{
    auto obj = new KcMarkerObject;
    super_::cloneTo_(*obj);
    obj->marker_ = marker_;
    return obj;
}


void KcMarkerObject::setMarker(const KpMarker& marker)
{
	// 判断是否需要重构vbo
	// 1. type不同，无条件重构
	// 2. type相同且hasOutline为false，则不须重构，只须更新shader参数
	// 3. type相同且hasOutline为true，若marker其他成员不同则重构

	bool rebuild = marker_.type != marker.type // 情况1，无条件重构
		|| (marker_.hasOutline() && marker != marker_); // 情况3

	marker_ = marker;
	setColor(marker_.fill);

	if (rebuild) {
		buildMarkerVbo_();
		prog_ = vbos_.front().decl->hasColor() ? KsShaderManager::singleton().progInst2dColor()
			: KsShaderManager::singleton().progInst2d();

		vbos_.resize(2);
		if (!vbos_[1].buf)
		    vbos_[1].buf = std::make_shared<KcGpuBuffer>();
		vbos_[1].decl = std::make_shared<KcVertexDeclaration>();
		auto loc = vbos_.front().decl->attributeCount();
		vbos_[1].decl->pushAttribute(KcVertexAttribute(loc, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_instance, 1));
	}
}


void KcMarkerObject::setOffset(const point3f* pos, unsigned count)
{
	assert(vbos_.size() > 1 && vbos_[1].buf && vbos_[1].decl);

	vbos_[1].buf->setData(pos, count * sizeof(point3f), KcGpuBuffer::k_stream_draw);
	instances_ = count;
}


void KcMarkerObject::buildMarkerVbo_()
{
	type_ = (marker_.type == KpMarker::k_dot) ? k_points : marker_.hasOutline() ? k_triangles : k_lines;

    if (vbos_.empty())
        vbos_.resize(1);
    if (!vbos_[0].buf)
        vbos_[0].buf = std::make_shared<KcGpuBuffer>();

    auto decl = std::make_shared<KcVertexDeclaration>();
    decl->pushAttribute(KcVertexAttribute::k_float2, KcVertexAttribute::k_position);
    if (marker_.hasOutline())
        decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);
    vbos_[0].decl = decl;

	auto vtx = markerVtx_(marker_.type);
	ibos_.clear();
	if (!marker_.hasOutline()) {
		vbos_[0].buf->setData(vtx.first, vtx.second * sizeof(point2f), KcGpuBuffer::k_stream_draw);
	}
	else {
		struct KpVertex
		{
			point2f pos;
			color4f clr;
		};

		bool fill = true; //  marker.fill.a() > 0;
		bool outline = marker_.showOutline && marker_.outline.a() > 0;
		std::vector<KpVertex> buf; buf.resize((fill + outline * 2) * vtx.second + fill);

		// 构造填充区域和轮廓区域
		auto pt = (const point2f*)vtx.first;
		KtLine<double> line({ pt[0].x(), pt[0].y(), 0 }, { pt[1].x() - pt[0].x(), pt[1].y() - pt[0].y(), 0 });
		auto f = 0.5 * std::round(marker_.weight) / (marker_.size * line.distanceTo(point3d(0))); // 轮廓线的缩放因子

		if (fill) {
			// 增加一个零点，以便构建三角形
			buf[0].pos = point2f(0);
			buf[0].clr = marker_.fill;
		}

		auto p = buf.data() + fill;
		for (unsigned i = 0; i < vtx.second; i++, p++) {
			// fill
			p[0].pos = pt[i];
			p[0].clr = marker_.fill;

			if (outline) {
				// outline
				p[vtx.second].pos = pt[i] * (1 - f);
				p[vtx.second].clr = marker_.outline;
				p[2 * vtx.second].pos = pt[i] * (1 + f);
				p[2 * vtx.second].clr = marker_.outline;
			}
		}

		vbos_[0].buf->setData(buf.data(), buf.size() * sizeof(KpVertex), KcGpuBuffer::k_stream_draw);

		buildIbo_(vtx.second);
	}
}


void KcMarkerObject::buildIbo_(unsigned vtxSize)
{
	bool fill = true; //  marker.fill.a() > 0;
	bool outline = marker_.hasOutline() && marker_.showOutline && marker_.outline.a() > 0;

	// 构造填充和轮廓的ibo
	// 为了整体绘制，整合填充和轮廓构建1个ibo
	std::vector<std::uint32_t> idx;
	auto ibo = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);

	// fill有vtxSize个三角形，共vtxSize * 3个顶点
	// outline有vtxSize * 2个三角形，共vtxSize * 2 * 3个顶点
	idx.resize((fill ? vtxSize * 3 : 0) + (outline ? vtxSize * 2 * 3 : 0));
	for (unsigned i = 0; i < vtxSize; i++) {
		if (fill) {
			idx[i * 3] = 0; // 零点
			idx[i * 3 + 1] = i + 1;
			idx[i * 3 + 2] = (i + 1) % vtxSize + 1;
		}

		if (outline) {

			assert(fill); // TODO:

			idx[vtxSize * 3 + i * 3] = 2 * vtxSize + i + 1;
			idx[vtxSize * 3 + i * 3 + 1] = 2 * vtxSize + (i + 1) % vtxSize + 1;
			idx[vtxSize * 3 + i * 3 + 2] = vtxSize + i + 1;


			idx[vtxSize * 6 + i * 3] = idx[vtxSize * 3 + i * 3 + 1];
			idx[vtxSize * 6 + i * 3 + 1] = vtxSize + (i + 1) % vtxSize + 1;
			idx[vtxSize * 6 + i * 3 + 2] = idx[vtxSize * 3 + i * 3 + 2];
		}
	}
	ibo->setData(idx.data(), idx.size() * sizeof(std::uint32_t), KcGpuBuffer::k_stream_draw);

	assert(ibos_.empty());
	pushIbo(ibo, idx.size());
}


bool KcMarkerObject::isSolidColor() const
{
	return !marker_.hasOutline() || !marker_.showOutline;
	bool fill = true; // TODO: 暂认为始终填充 marker.fill.a() != 0;
	bool outline = marker_.hasOutline() && marker_.showOutline && marker_.outline.a();
	return !fill || !outline;
}


std::pair<const void*, unsigned> KcMarkerObject::markerVtx_(int type)
{
	static const double SQRT_2_2 = std::sqrt(2.) / 2.;
	static const double SQRT_3_2 = std::sqrt(3.) / 2.;

	switch (type)
	{
	case KpMarker::k_dot:
	{
		static const point2f dot(0);
		return { &dot, 1 };
	}

	case KpMarker::k_cross:
	{
		static const point2f cross[4] = {
			point2f(-SQRT_2_2,-SQRT_2_2),
			point2f(SQRT_2_2,SQRT_2_2),
			point2f(SQRT_2_2,-SQRT_2_2),
			point2f(-SQRT_2_2,SQRT_2_2)
		};
		return { &cross, 4 };
	}

	case KpMarker::k_plus:
	{
		static const point2f plus[4] = { point2f(-1, 0), point2f(1, 0), point2f(0, -1), point2f(0, 1) };
		return { &plus, 4 };
	}

	case KpMarker::k_asterisk:
	{
		static const point2f asterisk[6] = {
			point2f(-SQRT_3_2, -0.5f),
			point2f(SQRT_3_2, 0.5f),
			point2f(-SQRT_3_2, 0.5f),
			point2f(SQRT_3_2, -0.5f),
			point2f(0, -1),
			point2f(0, 1)
		};
		return { &asterisk, 6 };
	}

	case KpMarker::k_square:
		return { KuPrimitiveFactory::square<float>(), 4 };

	case KpMarker::k_diamond:
		return { KuPrimitiveFactory::diamond<float>(), 4 };

	case KpMarker::k_left:
		return { KuPrimitiveFactory::triangleLeft<float>(), 3 };

	case KpMarker::k_right:
		return { KuPrimitiveFactory::triangleRight<float>(), 3 };

	case KpMarker::k_up:
		return { KuPrimitiveFactory::triangleUp<float>(), 3 };

	case KpMarker::k_down:
		return { KuPrimitiveFactory::triangleDown<float>(), 3 };

	case KpMarker::k_circle:
		return { KuPrimitiveFactory::circle10<float>(), 10 };

	default:
		break;
	};

	assert(false);
	return { nullptr, 0 };
}
