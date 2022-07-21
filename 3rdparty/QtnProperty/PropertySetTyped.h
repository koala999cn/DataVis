#pragma once
#include "PropertySet.h"


class QTN_IMPORT_EXPORT QtnPropertySetTyped : public QtnPropertySet
{
	Q_OBJECT

public:

	QtnPropertySetTyped(QtnProperty *typedProperty, QObject *parent = nullptr)
		: QtnPropertySet(parent) {
		m_typedProperty = typedProperty;
		connect(m_typedProperty, &QtnProperty::propertyDidChange,
			[this](QtnPropertyChangeReason reason) {
				if (reason & QtnPropertyChangeReasonStateLocal)
					setState(m_typedProperty->state());
			});
	}

	virtual ~QtnPropertySetTyped() override
	{
		delete m_typedProperty;
	}

    virtual QtnProperty* asProperty() override {
		return m_typedProperty;
	}

	virtual const QtnProperty* asProperty() const override	{
		return m_typedProperty;
	}

protected:

	virtual void doReset(QtnPropertyChangeReason reason) override {
		m_typedProperty->reset(reason);
		QtnPropertySet::reset(reason);
	}

	virtual bool event(QEvent* e) override {
		return ((QObject *) m_typedProperty)->event(e) && QtnPropertySet::event(e);
	}

	virtual bool loadImpl(QDataStream &stream) override {
		return m_typedProperty->load(stream) && QtnPropertySet::loadImpl(stream);
	}
	virtual bool saveImpl(QDataStream &stream) const override {
		return m_typedProperty->save(stream) && QtnPropertySet::saveImpl(stream);
	}

	virtual bool fromStrImpl(const QString &str, QtnPropertyChangeReason reason) override {
		return m_typedProperty->fromStr(str, reason);
	}
	virtual bool toStrImpl(QString &str) const override {
		return m_typedProperty->toStr(str);
	}

	virtual bool fromVariantImpl(
		const QVariant &var, QtnPropertyChangeReason reason) override {
		return m_typedProperty->fromVariant(var, reason);
	}
	virtual bool toVariantImpl(QVariant &var) const override {
		return m_typedProperty->toVariant(var);
	}

private:
	QtnProperty *m_typedProperty; 
};

#if 0
#include "PropertyCore.h"
#include "PropertyGUI.h"

template<typename T>
QtnProperty* QtnCreateProperty()
{
	if constexpr (std::is_same_v<T, bool>)
		return new QtnPropertyBool;
	else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
		return new QtnPropertyInt;
	else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
		return new QtnPropertyUInt;
	else if constexpr (std::is_same_v<T, float>)
		return new QtnPropertyFloat;
	else if constexpr (std::is_same_v<T, double>)
		return new QtnPropertyDouble;
	else 
	    return new QtnPropertyQVariant;
}


template<typename T>
class QtnPropertySetTyped : public QtnTypedPropertySetBase
{
public:
	QtnPropertySetTyped(QObject *parent = nullptr)
		: QtnTypedPropertySetBase(parent) {}

};

#endif