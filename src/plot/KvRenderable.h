#pragma once
#include <string_view>
#include "KtAABB.h"

class KvPaint;

class KvRenderable
{
public:
	using float_t = double;
	using aabb_t = KtAABB<float_t>;

	KvRenderable(const std::string_view& name) 
		: name_(name), visible_(true) {}

	const std::string& name() const { return name_; }
	void setName(const std::string_view& name) { 
		name_ = name; 
		onNameChanged();
	}

	bool visible() const { return visible_; }
	void setVisible(bool b) { 
		visible_ = b; 
		onVisibleChanged();
	}

	virtual void draw(KvPaint*) const = 0;

	// 统一语义：返回的aabb为物体局部坐标
	// NB: this可能并不知道如何转换到世界坐标，所以统一返回局部坐标
	virtual aabb_t boundingBox() const = 0;

	// 状态变更回调
	virtual void onNameChanged() {}
	virtual void onVisibleChanged() {}

private:
	std::string name_;
	bool visible_;
};
