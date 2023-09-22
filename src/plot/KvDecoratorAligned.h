#pragma once
#include "KvDecorator.h"
#include "layout/KvLayoutElement.h"


// 基于方框对齐的图饰，纳入layout体系

class KvDecoratorAligned : public KvDecorator, public KvLayoutElement
{
public:

	// 可选择对齐的目标
	enum KeAlignedTarget
	{
		k_coord_frame,
		k_coord_plane
	};


	KeAlignedTarget target() const { return target_; }
	KeAlignedTarget& target() { return target_; }

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	// 使用内框区域作为bounding-box
	virtual aabb_t boundingBox() const {
		auto& rc = innerRect();
		return { point3d(rc.lower().x(), rc.lower().y(), 0), 
			point3d(rc.upper().y(), rc.upper().y(), 0) };
	}

private:
	KeAlignedTarget target_;
	KeAlignment location_; // 对齐方式
};
