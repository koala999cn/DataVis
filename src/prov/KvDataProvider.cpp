#include "KvDataProvider.h"


KvDataProvider::kPropertySet KvDataProvider::propertySet() const
{
	kPropertySet ps;
	KpProperty prop;

	/// 所有属性皆为只读属性，id均取-1

	// TODO: 处理多维度和多通道
	
	// 类型
	prop.id = -1;
	prop.name = tr("Type");
	prop.val = isSampled() ? tr("sampled") : isContinued() ? tr("continued") : tr("scattered");
	prop.flag = k_readonly;
	ps.push_back(prop);

	// 维度
	prop.name = tr("Dim");
	prop.val = dim();
	ps.push_back(prop);

	// 通道数
	prop.name = tr("Channels");
	prop.val = channels();
	ps.push_back(prop);

	// 数据数量
	if (isDiscreted()) {
		prop.name = tr("Count");
		prop.desc = tr("count of data points");
		prop.val = count();
		ps.push_back(prop);
		prop.desc.clear();
	}

	// 采样间隔
	if (isSampled()) {
		prop.name = tr("Step");
		prop.val = step(0);
		ps.push_back(prop);

		prop.name = tr("SampleRate");
		prop.val = 1.0 / step(0);
		ps.push_back(prop);
	}

	// key range
	prop.name = "krange";
	prop.disp = tr("Key Range");
	prop.desc = tr("range of data keys");
	prop.val = QString("%1, %2").arg(range(0).low()).arg(range(0).high());
	ps.push_back(prop);

	// value range
	prop.name = "vrange";
	prop.disp = tr("Value Range");
	prop.desc = tr("range of data values");
	prop.val = QString("%1, %2").arg(range(dim()).low()).arg(range(dim()).high());
	ps.push_back(prop);

	return ps;
}
