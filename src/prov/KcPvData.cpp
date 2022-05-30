#include "KcPvData.h"
#include <QPointF>
#include "KvContinued.h"


KcPvData::KcPvData(const QString& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{

}


kIndex KcPvData::dim() const
{
	return data_->dim();
}


kIndex KcPvData::channels() const
{
	return data_->channels();
}


kRange KcPvData::range(kIndex axis) const
{
	return data_->range(axis);
}


kReal KcPvData::step(kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->step(axis) : 0;
}


kIndex KcPvData::size(kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->size(axis) : KvData::k_inf_size;
}


bool KcPvData::startImpl_()
{
	emit onData(data_);
	return true;
}


bool KcPvData::stopImpl_()
{
	return true; // TODO
}


bool KcPvData::running() const
{
	return false;
}


namespace kPrivate
{
	enum KeDataProperty
	{
		k_range
	};
}


KcPvData::kPropertySet KcPvData::propertySet() const
{
	auto ps = KvDataProvider::propertySet();

	// 替换ps中的range属性
	/* TODO:
	if (data_->isContinued()) {
		KpProperty prop, subProp;
		prop.id = kPrivate::k_range;
		prop.name = tr("Range");
		prop.flag = k_restrict;
		prop.val = QPointF(range(0).low(), range(0).high());
		subProp.name = tr("low");
		prop.children.push_back(subProp);
		subProp.name = QStringLiteral("high");
		prop.children.push_back(subProp);
		
		for (auto& p : ps) {
			if (p.name == tr("krange")) {
				p = prop;
				break;
			}
		}
	}*/

	return ps;
}


void KcPvData::setPropertyImpl_(int id, const QVariant& newVal)
{
	if (id == kPrivate::k_range) {

		auto pt = newVal.value<QPointF>();
		auto cnt = std::dynamic_pointer_cast<KvContinued>(data_);
		// TODO:
	}
}