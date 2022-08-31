#pragma once
#include "KvDataRender.h"
#include <QColor>


// 基于qwtplot3d实现的绘图类接口，主要实现以下功能：

class KvDataProvider;
class KvData;

namespace Qwt3D { class Plot3D; }


class KvRdQwtPlot3d : public KvDataRender
{
public:
	KvRdQwtPlot3d(KvDataProvider* is, const QString& name);
	virtual ~KvRdQwtPlot3d();

	kPropertySet propertySet() const override;

	void setOption(KeObjectOption opt, bool on) override;

	bool getOption(KeObjectOption opt) const override;

	QString exportAs();

	void showData();

	virtual void rescaleAxes();


protected:
	void setPropertyImpl_(int id, const QVariant& newVal) override;


protected:
	std::unique_ptr<Qwt3D::Plot3D> plot3d_;
};

