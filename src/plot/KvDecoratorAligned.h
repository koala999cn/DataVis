#pragma once
#include "KvDecorator.h"
#include "layout/KvLayoutElement.h"


// ���ڷ�������ͼ�Σ�����layout��ϵ

class KvDecoratorAligned : public KvDecorator, public KvLayoutElement
{
public:

	KvDecoratorAligned(const std::string_view& name) 
		: KvDecorator(name), KvLayoutElement() {}


	// ��ѡ������Ŀ��
	enum KeAlignedTarget
	{
		k_coord_frame,
		k_coord_plane
	};


	KeAlignedTarget alignedTarget() const { return target_; }
	KeAlignedTarget& alignedTarget() { return target_; }

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	// ʹ���ڿ�������Ϊbounding-box
	virtual aabb_t boundingBox() const {
		auto& rc = innerRect();
		return { point3d(rc.lower().x(), rc.lower().y(), 0), 
			point3d(rc.upper().x(), rc.upper().y(), 0) };
	}

private:
	KeAlignedTarget target_;
	KeAlignment location_; // ���뷽ʽ
};
