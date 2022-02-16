/*******************************************************************************
Copyright (c) 2012-2016 Alex Zhondin <lexxmark.dev@gmail.com>
Copyright (c) 2019 Alexandra Cherdantseva <neluhus.vagus@gmail.com>

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

#ifndef PROPERTY_BUTTON_H
#define PROPERTY_BUTTON_H

#include "QtnProperty/Property.h"
#include <functional>

class QStyleOptionButton;

class QTN_IMPORT_EXPORT QtnPropertyButton : public QtnProperty
{
	Q_OBJECT

public:
	explicit QtnPropertyButton(QObject *parent);

	void invokeClick();
	void setClickHandler(
		const std::function<void(const QtnPropertyButton *)> &clickHandler);

	inline QtnPropertyButton &operator=(const QtnPropertyButton &);

Q_SIGNALS:
	void click(const QtnPropertyButton *property);

public:
	void invokePreDrawButton(QStyleOptionButton *option);

Q_SIGNALS:
	void preDrawButton(
		const QtnPropertyButton *property, QStyleOptionButton *option);

protected:
	bool fromStrImpl(const QString &, QtnPropertyChangeReason) override;
	bool toStrImpl(QString &str) const override;
};

QtnPropertyButton &QtnPropertyButton::operator=(const QtnPropertyButton &)
{
	// do nothing
	return *this;
}

#endif // PROPERTY_BUTTON_H
