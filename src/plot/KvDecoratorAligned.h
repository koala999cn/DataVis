#pragma once
#include "KvDecorator.h"
#include "layout/KvLayoutElement.h"


// ���ڷ�������ͼ�Σ�����layout��ϵ

class KvDecoratorAligned : public KvDecorator, public KvLayoutElement
{
public:

	KvDecoratorAligned(const std::string_view& name) 
		: KvDecorator(name), KvLayoutElement() {}

	// ����Ĳ�����
	enum KeAlignTarget
	{
		k_plot_frame, // �����plot�߿���ж���
		k_coord_frame // ���������ϵ�߿���ж���
	};

	// �߿��ѡ��ģʽ
	enum KeBorderMode
	{
		k_outter, // ѡ��"��߿�"Ϊborder����KvLayoutElement::outterRect
		k_margins, // ѡ��"��߿�-����"Ϊborder����KvLayoutElement::outterRect - KvLayoutElement::margins
		k_inner // ѡ��"�ڱ߿�"Ϊborder����KvLayoutElement::innerRect
	};

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	KeBorderMode borderMode() const { return borderMode_; }
	KeBorderMode& borderMode() { return borderMode_; }

	KeAlignTarget alignTarget() const { return target_; }
	KeAlignTarget& alignTarget() { return target_; }

	// ʹ���ڿ�������Ϊbounding-box
	virtual aabb_t boundingBox() const {
		auto rc = (borderMode_ == k_inner) ? innerRect() : outterRect();
		if (borderMode_ == k_margins)
			rc.deflate(margins());
		return { point3d(rc.lower().x(), rc.lower().y(), 0), 
			point3d(rc.upper().x(), rc.upper().y(), 0) };
	}

private:
	KeAlignment location_; // ���뷽ʽ
	KeBorderMode borderMode_{ k_margins };
	KeAlignTarget target_{ k_coord_frame };
};
