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

#ifndef PROPERTYINT_H
#define PROPERTYINT_H

#include "QtnProperty/Auxiliary/PropertyTemplates.h"

class QTN_IMPORT_EXPORT QtnPropertyIntBase
	: public QtnNumericPropertyBase<QtnSinglePropertyBase<qint32>>
{
	Q_OBJECT

private:
	QtnPropertyIntBase(const QtnPropertyIntBase &other) Q_DECL_EQ_DELETE;

public:
	explicit QtnPropertyIntBase(QObject *parent);

protected:
	// string conversion implementation
	bool fromStrImpl(
		const QString &str, QtnPropertyChangeReason reason) override;
	bool toStrImpl(QString &str) const override;

	P_PROPERTY_DECL_MEMBER_OPERATORS(QtnPropertyIntBase)
};

P_PROPERTY_DECL_ALL_OPERATORS(QtnPropertyIntBase, qint32)

class QTN_IMPORT_EXPORT QtnPropertyIntCallback
	: public QtnSinglePropertyCallback<QtnPropertyIntBase>
{
	Q_OBJECT

private:
	QtnPropertyIntCallback(
		const QtnPropertyIntCallback &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyIntCallback(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(
		QtnPropertyIntCallback, QtnPropertyIntBase)
};

class QTN_IMPORT_EXPORT QtnPropertyInt
	: public QtnNumericPropertyValue<QtnPropertyIntBase>
{
	Q_OBJECT

private:
	QtnPropertyInt(const QtnPropertyInt &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyInt(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyInt, QtnPropertyIntBase)
};

#endif // PROPERTYINT_H
