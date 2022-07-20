#pragma once
#include "PropertySet.h"


class QTN_IMPORT_EXPORT QtnTypedPropertySet : public QtnPropertySet
{
	Q_OBJECT

public:

	QtnTypedPropertySet(QtnProperty *typedProperty, QObject *parent = nullptr)
		: QtnPropertySet(parent) {
		m_typedProperty = typedProperty;
	}

	virtual ~QtnTypedPropertySet() override
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
	}

	virtual bool event(QEvent* e) override {
		return ((QObject*)m_typedProperty)->event(e);
	}

	virtual bool loadImpl(QDataStream &stream) override {
		return m_typedProperty->load(stream);
	}
	virtual bool saveImpl(QDataStream &stream) const override {
		return m_typedProperty->save(stream);
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

#if 0
	virtual void updateStateInherited(bool force) override {
		return m_typedProperty->updateStateInherited(force);
	}
	void setStateInherited(QtnPropertyState stateToSet, bool force = false) override {
		return m_typedProperty->setStateInherited(stateToSet, force);
	}

	void setStateInternal(QtnPropertyState stateToSet, bool force = false,
		QtnPropertyChangeReason reason = QtnPropertyChangeReason()) override {
		return m_typedProperty->setStateInternal(stateToSet, force, reason);
	}

	virtual void masterPropertyWillChange(QtnPropertyChangeReason reason) override {
		return m_typedProperty->masterPropertyWillChange(reason);
	}
	virtual void masterPropertyDidChange(QtnPropertyChangeReason reason) override {
		return m_typedProperty->masterPropertyDidChange(reason);
	}

	virtual void updatePropertyState() override {
		return m_typedProperty->updatePropertyState();
	}
#endif

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
class QtnTypedPropertySet : public QtnTypedPropertySetBase
{
public:
	QtnTypedPropertySet(QObject *parent = nullptr)
		: QtnTypedPropertySetBase(parent) {}

};

#endif