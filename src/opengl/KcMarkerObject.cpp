#include "KcMarkerObject.h"
#include "glad.h"
#include "KcGlslProgram.h"
#include "KsShaderManager.h"
#include "KcVertexDeclaration.h"
#include "KuPrimitiveFactory.h"
#include "KcGpuBuffer.h"
#include "KtLine.h"


KcMarkerObject::KcMarkerObject()
    : super_(k_points) // ��ʼ�����޹ؽ�Ҫ������������marker��������
{
	marker_.type = -1;

	// ��4��vbo
	// loc0: �洢marker�ı�׼�������ݣ�float4��
	// loc1: �洢��ʵ����λ�����ݣ�float3��
	// loc2: �洢��ʵ���ĳߴ����ݣ�float������ѡ 
	// loc3: �洢��ʵ�������ɫ���ݣ�float4������ѡ
	vbos_.resize(4);

	vbos_[0].decl = std::make_shared<KcVertexDeclaration>();
	vbos_[0].decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_position);

	vbos_[1].decl = std::make_shared<KcVertexDeclaration>();
	vbos_[1].decl->pushAttribute(KcVertexAttribute(1, KcVertexAttribute::k_float3, 0, KcVertexAttribute::k_instance, 1));

	vbos_[2].decl = std::make_shared<KcVertexDeclaration>();
	vbos_[2].decl->pushAttribute(KcVertexAttribute(2, KcVertexAttribute::k_float, 0, KcVertexAttribute::k_instance, 1));

	vbos_[3].decl = std::make_shared<KcVertexDeclaration>();
	vbos_[3].decl->pushAttribute(KcVertexAttribute(3, KcVertexAttribute::k_float4, 0, KcVertexAttribute::k_instance, 1));

	for (unsigned i = 0; i < vbos_.size(); i++)
		vbos_[i].buf = std::make_shared<KcGpuBuffer>();
}


void KcMarkerObject::draw() const
{
	prog_->useProgram();

	auto& sm = KsShaderManager::singleton();
	prog_->setUniform(sm.varname(KsShaderManager::k_minor_color), marker_.outline);

	int bSizeVarying = vbos_[2].decl->getAttribute(0).enabled();
	prog_->setUniform(sm.varname(KsShaderManager::k_inst_size_varying), bSizeVarying);

	int bColorVarying = vbos_[3].decl->getAttribute(0).enabled();
	prog_->setUniform(sm.varname(KsShaderManager::k_inst_color_varying), bColorVarying);

	if (type_ == k_points) {
		glPointSize(marker_.size);
	}
	else {
		if (bSizeVarying) 
			prog_->setUniform(sm.varname(KsShaderManager::k_inst_scale), scale_);
		else 
			prog_->setUniform(sm.varname(KsShaderManager::k_inst_scale), scale_ * marker_.size);
	}

	// vColor��matMvp����ֵ�ɻ�������
    super_::draw();
}


KcRenderObject* KcMarkerObject::clone() const
{
    auto obj = new KcMarkerObject;
    super_::cloneTo_(*obj);
    obj->marker_ = marker_;
	obj->scale_ = scale_;
    return obj;
}


void KcMarkerObject::setMarker(const KpMarker& marker)
{
	// �ж��Ƿ���Ҫ�ع�vbo
	// 1. type��ͬ���������ع�
	// 2. type��ͬ��hasOutlineΪfalse�������ع���ֻ�����shader����
	// 3. type��ͬ��hasOutlineΪtrue����marker������Ա��ͬ���ع�

	bool rebuild = marker_.type != marker.type // ���1���������ع�
		|| (marker_.hasOutline() && marker != marker_); // ���3

	marker_ = marker;
	setColor(marker_.fill);

	if (rebuild) {
		buildMarkerVbo_();
		prog_.reset(); // ���ڲ�֪��flat��clipBox״̬�������ⲿ�Զ�����
	}
}


void KcMarkerObject::setInstPos(const point3f* pos, unsigned count)
{
	assert(vbos_[1].buf);

	vbos_[1].buf->setData(pos, count * sizeof(point3f), KcGpuBuffer::k_stream_draw);
	instances_ = count;
}


void KcMarkerObject::setInstSize(const float* size)
{
	assert(vbos_[2].decl && vbos_[2].decl->vertexSize() == sizeof(float));
	vbos_[2].buf->setData(size, size ? instances_ * sizeof(float) : 0, KcGpuBuffer::k_stream_draw);
	vbos_[2].decl->getAttribute(0).enable(size);
}


void KcMarkerObject::setInstColor(const color4f* clr)
{
	assert(vbos_[3].decl && vbos_[3].decl->vertexSize() == sizeof(color4f));
	vbos_[3].buf->setData(clr, clr ? instances_ * sizeof(color4f) : 0, KcGpuBuffer::k_stream_draw);
	vbos_[3].decl->getAttribute(0).enable(clr);
}


bool KcMarkerObject::realShowOutline_() const
{
	return marker_.hasOutline() && marker_.showOutline && 
		(marker_.outline.a() != 0 || !marker_.showFill);
}


void KcMarkerObject::buildMarkerVbo_()
{
	type_ = (marker_.type == KpMarker::k_dot) ? k_points : marker_.hasOutline() ? k_triangles : k_lines;

    if (!vbos_[0].buf)
        vbos_[0].buf = std::make_shared<KcGpuBuffer>();

	auto vtx = markerVtx_(marker_.type);
	auto pt = (const point2f*)vtx.first;
	
	std::vector<float4> pos;
	
	double f(0); // �����ߵ���������
	if (realShowOutline_()) {
		KtLine<double> line({ pt[0].x(), pt[0].y(), 0 }, { pt[1].x() - pt[0].x(), pt[1].y() - pt[0].y(), 0 });
		f = 0.5 * std::round(marker_.weight) / (marker_.size * line.distanceTo(point3d(0))); 
	}

	if (marker_.showFill) {
		int headingZero = (type_ == k_triangles) ? 1 : 0; // �Ƿ�������ĵ㣨0��0��0��
		pos.resize(vtx.second + headingZero);
		auto p = pt;
		for (unsigned i = headingZero; i < pos.size(); i++, p++)
			pos[i] = { p->x() * (1 - f), p->y() * (1 - f), 0, 0 }; // ����0�����Ϊfill����

		if (headingZero)
			pos[0] = point4f(0);
	}

	if (f != 0) {
		pos.resize(pos.size() + 2 * vtx.second);

		float w = marker_.showFill ? 1 : 0;
		auto p = pos.data() + pos.size() - 2 * vtx.second;
		for (unsigned i = 0; i < vtx.second; i++, p++) {
			p[0] = { pt[i].x() * (1 - f), pt[i].y() * (1 - f), 0, w }; // ����1�����Ϊoutline����
			p[vtx.second] = { pt[i].x() * (1 + f), pt[i].y() * (1 + f), 0, w };
		}
	}

	assert(!pos.empty());
	vbos_[0].buf->setData(pos.data(), pos.size() * sizeof(point4f), KcGpuBuffer::k_stream_draw);

	if (type_ == k_triangles)
	    buildIbo_(vtx.second);
	else 
		ibos_.clear();
}


void KcMarkerObject::buildIbo_(unsigned vtxSize)
{
	// ��������������ibo
	// Ϊ��������ƣ�����������������1��ibo
	std::vector<std::uint32_t> idx;
	auto ibo = std::make_shared<KcGpuBuffer>(KcGpuBuffer::k_index_buffer);

	// fill��vtxSize�������Σ���vtxSize * 3������
	if (marker_.showFill) {
		idx.resize(vtxSize * 3);
		for (unsigned i = 0; i < vtxSize; i++) {
			idx[i * 3] = 0; // ���
			idx[i * 3 + 1] = i + 1;
			idx[i * 3 + 2] = (i + 1) % vtxSize + 1;
		}
	}

	// outline��vtxSize * 2�������Σ���vtxSize * 2 * 3������
	if (realShowOutline_()) {
		idx.resize(idx.size() + vtxSize * 2 * 3);
		auto p0 = idx.data() + idx.size() - vtxSize * 2 * 3;
		auto p1 = p0 + vtxSize * 3;
		unsigned baseIdx = marker_.showFill ? vtxSize + 1/*���ĵ�*/ : 0;
		for (unsigned i = 0; i < vtxSize; i++) {
			p0[i * 3] = baseIdx + vtxSize + i;
			p0[i * 3 + 1] = baseIdx + vtxSize + (i + 1) % vtxSize;
			p0[i * 3 + 2] = baseIdx + i;


			p1[i * 3] = p0[i * 3 + 1];
			p1[i * 3 + 1] = baseIdx + (i + 1) % vtxSize;
			p1[i * 3 + 2] = p0[i * 3 + 2];
		}
	}

	ibo->setData(idx.data(), idx.size() * sizeof(std::uint32_t), KcGpuBuffer::k_stream_draw);

	ibos_.clear();
	pushIbo(ibo, idx.size());
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
		return { KuPrimitiveFactory::circle50<float>(), 50 }; // TODO: Ӧ����marker�ߴ��Զ����õ���

	default:
		break;
	};

	assert(false);
	return { nullptr, 0 };
}
