#pragma once
#include <memory>
#include <vector>
#include "KtColor.h"
#include "KvPlottable.h"
#include "KcCoordSystem.h"

class KvPlot
{
public:

	KvPlot();

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void* widget() const = 0;

	virtual color4f background() const = 0;
	virtual void setBackground(const color4f& clr) = 0;

	// ���»�ͼ
	virtual void update();

	KcCoordSystem& coordSystem() {
		return *coord_.get();
	}

	unsigned plottableCount() const {
		return plottables_.size();
	}

	KvPlottable* plottable(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plot);

	bool isOrtho() const { return ortho_; }
	void setOrtho(bool b) { ortho_ = b; }

	bool isFitData() const { return fitData_; }
	void setFitData(bool b) { fitData_ = b; }

	bool isIsometric() const { return isometric_; }
	void setIsometric(bool b) { isometric_ = b; }

	double getZoom() const { return zoom_; }
	void setZoom(double zoom) { zoom_ = zoom; }
	void zoom(double factor) { zoom_ *= factor; }

	point3d getScale() const { return scale_; }
	void setScale(const point3d& scale) { scale_ = scale; }
	void scale(const point3d& factor) { scale_ *= factor; }

	point3d getShift() const { return shift_; }
	void setShift(const point3d& shift) { shift_ = shift; }
	void shift(const point3d& delta) { shift_ += delta; }

	point3d getRotate() const { return rotate_; }
	void setRotate(const point3d& rotate) { rotate_ = rotate; }
	void rotate(const point3d& delta) { rotate_ += delta; }

protected:
	virtual void updateImpl_() = 0;
	virtual void autoProject_() = 0;
	virtual void autoFit_();
	
protected:
	std::unique_ptr<KcCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
	bool ortho_; // ����ͶӰ vs. ͸��ͶӰ
	bool fitData_; // ��true����ÿ��update������������range�Զ���������ϵextents

	// ���²������ڵ��������modelview����
	double zoom_;
	point3d scale_;
	point3d shift_;
	point3d rotate_;

	bool isometric_; // ��true���򱣳�����ϵ�ĵȱ��ԣ���x���1����Ԫ����ʼ�յ���y/z���1����Ԫ����
	                 // ��false�������ȿ��ǲ��������ԣ�����ϵ�ĳ�ʼ͸�ӽ��ʼ��Ϊ�����Σ�������ͨ��scale_�����������죩
};
