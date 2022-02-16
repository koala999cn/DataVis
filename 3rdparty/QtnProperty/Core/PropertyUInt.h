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

#ifndef PROPERTYUINT_H
#define PROPERTYUINT_H

#include "QtnProperty/Auxiliary/PropertyTemplates.h"

class QTN_IMPORT_EXPORT QtnPropertyUIntBase
	: public QtnNumericPropertyBase<QtnSinglePropertyBase<quint32>>
{
	Q_OBJECT

private:
	QtnPropertyUIntBase(const QtnPropertyUIntBase &other) Q_DECL_EQ_DELETE;

public:
	explicit QtnPropertyUIntBase(QObject *parent);

protected:
	// string conversion implementation
	bool fromStrImpl(
		const QString &str, QtnPropertyChangeReason reason) override;
	bool toStrImpl(QString &str) const override;

	P_PROPERTY_DECL_MEMBER_OPERATORS(QtnPropertyUIntBase)
};

P_PROPERTY_DECL_ALL_OPERATORS(QtnPropertyUIntBase, quint32)

class QTN_IMPORT_EXPORT QtnPropertyUIntCallback
	: public QtnSinglePropertyCallback<QtnPropertyUIntBase>
{
	Q_OBJECT

private:
	QtnPropertyUIntCallback(
		const QtnPropertyUIntCallback &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyUIntCallback(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(
		QtnPropertyUIntCallback, QtnPropertyUIntBase)
};

class QTN_IMPORT_EXPORT QtnPropertyUInt
	: public QtnNumericPropertyValue<QtnPropertyUIntBase>
{
	Q_OBJECT

private:
	QtnPropertyUInt(const QtnPropertyUInt &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyUInt(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyUInt, QtnPropertyUIntBase)
};

#endif // PROPERTYUINT_H
