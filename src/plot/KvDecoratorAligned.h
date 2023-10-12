#pragma once
#include "KvDecorator.h"
#include "layout/KvLayoutElement.h"


// 基于方框对齐的图饰，纳入layout体系

class KvDecoratorAligned : public KvDecorator, public KvLayoutElement
{
public:

	KvDecoratorAligned(const std::string_view& name) 
		: KvDecorator(name), KvLayoutElement() {}

	// 边框的选择模式
	enum KeBorderMode
	{
		k_outter, // 选择"外边框"为border，即KvLayoutElement::outterRect
		k_margins, // 选择"外边框-留白"为border，即KvLayoutElement::outterRect - KvLayoutElement::margins
		k_inner // 选择"内边框"为border，即KvLayoutElement::innerRect
	};

	KeAlignment location() const { return location_; }
	KeAlignment& location() { return location_; }

	KeBorderMode borderMode() const { return borderMode_; }
	KeBorderMode& borderMode() { return borderMode_; }

	// 使用内框区域作为bounding-box
	virtual aabb_t boundingBox() const {
		auto rc = (borderMode_ == k_inner) ? innerRect() : outterRect();
		if (borderMode_ == k_margins)
			rc.deflate(margins());
		return { point3d(rc.lower().x(), rc.lower().y(), 0), 
			point3d(rc.upper().x(), rc.upper().y(), 0) };
	}

private:
	KeAlignment location_; // 对齐方式
	KeBorderMode borderMode_{ k_margins };
};
