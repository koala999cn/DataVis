#pragma once
#include <memory>
#include <vector>
#include "KvRenderable.h"
#include "KvCoordSystem.h"
#include "KvPlottable.h"


// ��ͼ���ڳ�����

class KvPlotWidget : public KvRenderable
{
public:
	using widget_t = void*;

	virtual widget_t* widget() const = 0; // ������KvPlotWidget�ҽӵ�win����ָ��

	// ���»�ͼ����������geometry��effect
	virtual void update(bool immediately = true) = 0;

private:
	std::unique_ptr<KvCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
};
