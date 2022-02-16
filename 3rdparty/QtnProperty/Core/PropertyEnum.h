/*******************************************************************************
Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>
Copyright (c) 2015-2019 Alexandra Cherdantseva <neluhus.vagus@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#ifndef PROPERTYENUM_H
#define PROPERTYENUM_H

#include "QtnProperty/Auxiliary/PropertyTemplates.h"
#include "QtnProperty/Enum.h"

class QTN_IMPORT_EXPORT QtnPropertyEnumBase
	: public QtnSinglePropertyBase<QtnEnumValueType>
{
	Q_OBJECT

private:
	QtnPropertyEnumBase(const QtnPropertyEnumBase &other) Q_DECL_EQ_DELETE;

public:
	explicit QtnPropertyEnumBase(QObject *parent);

	inline const QtnEnumInfo *enumInfo() const;

	inline void setEnumInfo(const QtnEnumInfo *enumInfo);

protected:
	// string conversion implementation
	bool fromStrImpl(
		const QString &str, QtnPropertyChangeReason reason) override;
	bool toStrImpl(QString &str) const override;

	bool isValueAcceptedImpl(ValueType valueToAccept) override;

private:
	const QtnEnumInfo *m_enumInfo;

	P_PROPERTY_DECL_MEMBER_OPERATORS(QtnPropertyEnumBase)
};

const QtnEnumInfo *QtnPropertyEnumBase::enumInfo() const
{
	return m_enumInfo;
}

void QtnPropertyEnumBase::setEnumInfo(const QtnEnumInfo *enumInfo)
{
	m_enumInfo = enumInfo;
}

P_PROPERTY_DECL_ALL_OPERATORS(QtnPropertyEnumBase, QtnEnumValueType)

class QTN_IMPORT_EXPORT QtnPropertyEnumCallback
	: public QtnSinglePropertyCallback<QtnPropertyEnumBase>
{
	Q_OBJECT

private:
	QtnPropertyEnumCallback(
		const QtnPropertyEnumCallback &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyEnumCallback(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(
		QtnPropertyEnumCallback, QtnPropertyEnumBase)
};

class QTN_IMPORT_EXPORT QtnPropertyEnum
	: public QtnSinglePropertyValue<QtnPropertyEnumBase>
{
	Q_OBJECT QtnPropertyEnum(const QtnPropertyEnum &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyEnum(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyEnum, QtnPropertyEnumBase)
};

#endif // PROPERTYENUM_H
