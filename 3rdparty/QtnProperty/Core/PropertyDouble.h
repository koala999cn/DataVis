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

#ifndef PROPERTYDOUBLE_H
#define PROPERTYDOUBLE_H

#include "QtnProperty/Auxiliary/PropertyTemplates.h"

class QTN_IMPORT_EXPORT QtnPropertyDoubleBase
	: public QtnNumericPropertyBase<QtnSinglePropertyBase<double>>
{
	Q_OBJECT

private:
	QtnPropertyDoubleBase(const QtnPropertyDoubleBase &other) Q_DECL_EQ_DELETE;

public:
	explicit QtnPropertyDoubleBase(QObject *parent);

protected:
	// string conversion implementation
	bool fromStrImpl(
		const QString &str, QtnPropertyChangeReason reason) override;
	bool toStrImpl(QString &str) const override;

	P_PROPERTY_DECL_MEMBER_OPERATORS(QtnPropertyDoubleBase)
};

P_PROPERTY_DECL_ALL_OPERATORS(QtnPropertyDoubleBase, double)

class QTN_IMPORT_EXPORT QtnPropertyDoubleCallback
	: public QtnSinglePropertyCallback<QtnPropertyDoubleBase>
{
	Q_OBJECT

private:
	QtnPropertyDoubleCallback(
		const QtnPropertyDoubleCallback &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyDoubleCallback(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(
		QtnPropertyDoubleCallback, QtnPropertyDoubleBase)
};

class QTN_IMPORT_EXPORT QtnPropertyDouble
	: public QtnNumericPropertyValue<QtnPropertyDoubleBase>
{
	Q_OBJECT

private:
	QtnPropertyDouble(const QtnPropertyDouble &other) Q_DECL_EQ_DELETE;

public:
	Q_INVOKABLE explicit QtnPropertyDouble(QObject *parent = nullptr);

	P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyDouble, QtnPropertyDoubleBase)
};

#endif // PROPERTYDOUBLE_H
