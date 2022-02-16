#include "QtnPropertyDelegatePropertySetX.h"
#include "QtnProperty/PropertyView.h"
#include <QKeyEvent>


QtnPropertyDelegatePropertySetX::QtnPropertyDelegatePropertySetX(QtnPropertySetX& owner) 
	: QtnPropertyDelegatePropertySet(owner)
{

}


void QtnPropertyDelegatePropertySetX::Register(QtnPropertyDelegateFactory& factory)
{
	factory.registerDelegateDefault(&QtnPropertySetX::staticMetaObject,
		&qtnCreateDelegate<QtnPropertyDelegatePropertySetX, QtnPropertySetX>,
		"Default");
}


void QtnPropertyDelegatePropertySetX::createSubItemsImpl(
	QtnDrawContext& context, QList<QtnSubItem>& subItems)
{
	QtnPropertyDelegatePropertySet::createSubItemsImpl(context, subItems);
	addSubItemValues(context, subItems);
}


void QtnPropertyDelegatePropertySetX::addSubItemValues(
	QtnDrawContext& context, QList<QtnSubItem>& subItems)
{
	auto rect = context.rect.marginsRemoved(context.margins);
	rect.setLeft(context.splitPos);

	if (stateProperty()->isResettable() && !stateProperty()->valueIsDefault())
	{
		rect.setRight(rect.right() - rect.height());
	}

	if (rect.isValid())
		createSubItemValuesImpl(context, rect, subItems);
}


void QtnPropertyDelegatePropertySetX::createSubItemValuesImpl(
	QtnDrawContext& context, const QRect& valueRect, QList<QtnSubItem>& subItems)
{
	QtnSubItem subItem(valueRect);
	if (createSubItemValueImpl(context, subItem))
		subItems.append(subItem);
}


bool QtnPropertyDelegatePropertySetX::createSubItemValueImpl(
	QtnDrawContext& context, QtnSubItem& subItemValue)
{
	return createSubItemValueBoolCheck(context, subItemValue);

	auto type = owner().value().type();
	switch (type)
	{
	case QMetaType::Bool:
		return createSubItemValueBoolCheck(context, subItemValue);

	case QMetaType::UnknownType:
	default:
		break;
	}

	return true;
}


bool QtnPropertyDelegatePropertySetX::createSubItemValueBoolCheck(QtnDrawContext& context, QtnSubItem& subItemValue)
{
	subItemValue.trackState();
	subItemValue.rect.adjust(context.widget->valueLeftMargin(), 0, 0, 0);
	subItemValue.rect.setWidth(
		context.style()->pixelMetric(QStyle::PM_IndicatorWidth));

	subItemValue.drawHandler = [this](QtnDrawContext& context,
		const QtnSubItem& item) {
			QStyleOptionButton opt;
			opt.rect = item.rect;
			opt.state = state(context.isActive, item);

			if (stateProperty()->isMultiValue())
			{
				opt.state |= QStyle::State_NoChange;
			}
			else
			{
				bool value = owner().value().toBool();
				opt.state |= (value ? QStyle::State_On : QStyle::State_Off);
			}

			context.painter->drawControl(QStyle::CE_CheckBox, opt);
	};

	subItemValue.eventHandler = [this](QtnEventContext& context,
		const QtnSubItem&,
		QtnPropertyToEdit* toEdit) {
			bool toggleValue = false;
			switch (context.eventType())
			{
			case QEvent::MouseButtonRelease:
				toggleValue = true;
				break;

			case QEvent::KeyPress:
			{
				int key = context.eventAs<QKeyEvent>()->key();
				toggleValue = (key == Qt::Key_Space) || (key == Qt::Key_Return);
				break;
			}

			default:
				return false;
			}

			if (toggleValue)
			{
				toEdit->setup(property(), [this]() -> QWidget* {
					QtnPropertyDelegatePropertySetX* thiz = this;
					auto& p = thiz->owner();
					p.setValue(!p.value().toBool(), editReason());
					return nullptr;
					});
			}

			return true;
	};

	return true;
}
