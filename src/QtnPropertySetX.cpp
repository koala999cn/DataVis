#include "QtnPropertySetX.h"


bool QtnPropertySetX::fromStrImpl(const QString& str, QtnPropertyChangeReason reason) {
	if (str.compare("true", Qt::CaseInsensitive) == 0) {
		m_checked = true;
		return true;
	}
	else if (str.compare("false", Qt::CaseInsensitive) == 0) {
		m_checked = false;
		return true;
	}

	return false;
}

bool QtnPropertySetX::toStrImpl(QString& str) const {
	str = m_checked ? "True" : "False";
	return true;
}


bool QtnPropertySetX::fromVariantImpl(
	const QVariant& var, QtnPropertyChangeReason reason) {
	if (var.type() == QMetaType::Bool) {
		m_checked = var.toBool();
		return true;
	}

	return false;
}

bool QtnPropertySetX::toVariantImpl(QVariant& var) const {
	var = QVariant(m_checked);
	return true;
}


// TODO: 可改造QtnSinglePropertyBase
bool QtnPropertySetX::setValue(const QVariant& newValue, QtnPropertyChangeReason reason)
{
	if ((reason & QtnPropertyChangeReasonEdit) && !isEditableByUser())
		return false;

	if (!isWritable()/* || !isValueAcceptedImpl(newValue)*/)
		return false;

	if (newValue.type() != QMetaType::Bool)
		return false;

	if (!(reason & QtnPropertyChangeReasonMultiEdit) && m_checked == newValue.toBool())
		return true;

	//bool accept = true;
	//emit propertyValueAccept(QtnPropertyValuePtr(&newValue), &accept);

	//if (!accept)
	//	return false;

	if (!(reason & QtnPropertyChangeReasonValue))
		reason |= QtnPropertyChangeReasonNewValue;

	emit propertyWillChange(reason, QtnPropertyValuePtr(&newValue), QMetaType::Bool);
	fromVariant(newValue, reason);
	emit propertyDidChange(reason);

	return true;
}