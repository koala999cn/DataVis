#pragma once
#include "QtnProperty/PropertySet.h"
#include <QVariant>


class QtnPropertySetX : public QtnPropertySet
{
	Q_OBJECT

public:
	using QtnPropertySet::QtnPropertySet;

	QVariant value() const { 
		QVariant var;
		toVariantImpl(var);
		return var;
	}

	bool setValue(const QVariant& val, QtnPropertyChangeReason reason = QtnPropertyChangeReason());

protected:

	bool fromStrImpl(const QString& str, QtnPropertyChangeReason reason);
	bool toStrImpl(QString& str) const;

	bool fromVariantImpl(const QVariant& var, QtnPropertyChangeReason reason);
	bool toVariantImpl(QVariant& var) const;

private:
	bool m_checked;
};

