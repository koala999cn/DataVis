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

#ifndef QTN_PROPERTY_DELEGATE_FACTORY_H
#define QTN_PROPERTY_DELEGATE_FACTORY_H

#include "PropertyDelegate.h"
#include <QMap>

#include <functional>

class QTN_IMPORT_EXPORT QtnPropertyDelegateFactory
{
	Q_DISABLE_COPY(QtnPropertyDelegateFactory)

public:
	using CreateFunction =
		std::function<QtnPropertyDelegate *(QtnPropertyBase &)>;

	explicit QtnPropertyDelegateFactory(
		QtnPropertyDelegateFactory *superFactory = nullptr);

	static void registerDefaultDelegates(QtnPropertyDelegateFactory &factory);

	inline QtnPropertyDelegateFactory *superFactory();
	void setSuperFactory(QtnPropertyDelegateFactory *superFactory);

	QtnPropertyDelegate *createDelegate(QtnPropertyBase &owner);

	bool registerDelegateDefault(const QMetaObject *propertyMetaObject,
		const CreateFunction &createFunction,
		const QByteArray &delegateName = QByteArray());
	bool registerDelegate(const QMetaObject *propertyMetaObject,
		const CreateFunction &createFunction, const QByteArray &delegateName);

	bool unregisterDelegate(const QMetaObject *propertyMetaObject);
	bool unregisterDelegate(
		const QMetaObject *propertyMetaObject, const QByteArray &delegateName);

	static QtnPropertyDelegateFactory &staticInstance();
	static void resetDefaultInstance(QtnPropertyDelegateFactory *factory);

private:
	QtnPropertyDelegate *createDelegateInternal(QtnPropertyBase &owner);

	QtnPropertyDelegateFactory *m_superFactory;

	struct CreateItem
	{
		CreateFunction defaultCreateFunction;
		QMap<QByteArray, CreateFunction> createFunctions;
	};

	QMap<const QMetaObject *, CreateItem> m_createItems;
};

QtnPropertyDelegateFactory *QtnPropertyDelegateFactory::superFactory()
{
	return m_superFactory;
}

template <typename PropertyDelegateClass, typename PropertyClass>
QtnPropertyDelegate *qtnCreateDelegate(QtnPropertyBase &owner)
{
	PropertyClass *theOwner = qobject_cast<PropertyClass *>(&owner);
	if (!theOwner)
		return nullptr;

	return new PropertyDelegateClass(*theOwner);
}

#endif // QTN_PROPERTY_DELEGATE_FACTORY_H
