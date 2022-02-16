#pragma once
#include "QtnProperty/Delegates/Utils/PropertyDelegatePropertySet.h"
#include "QtnPropertySetX.h"


// QtnPropertyDelegatePropertySet with checkbox and text support
class QtnPropertyDelegatePropertySetX : public QtnPropertyDelegatePropertySet
{
public:
	QtnPropertyDelegatePropertySetX(QtnPropertySetX& owner);

	static void Register(QtnPropertyDelegateFactory& factory);

	const QtnPropertySetX& owner() const
	{
		return *static_cast<const QtnPropertySetX*>(this->propertyImmutable());
	}
	QtnPropertySetX& owner()
	{
		return *static_cast<QtnPropertySetX*>(this->property());
	}

protected:

	void createSubItemsImpl(
		QtnDrawContext& context, QList<QtnSubItem>& subItems) override;

	void addSubItemValues(
		QtnDrawContext& context, QList<QtnSubItem>& subItems);

	void createSubItemValuesImpl(
		QtnDrawContext& context, const QRect& valueRect, QList<QtnSubItem>& subItems);

	bool createSubItemValueImpl(
		QtnDrawContext& context, QtnSubItem& subItemValue);

private:
	bool createSubItemValueBoolCheck(QtnDrawContext& context, QtnSubItem& subItemValue);
};

