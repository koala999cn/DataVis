#pragma once
#include "KvRenderable.h"
#include <string>
#include <QColor>


class KvPlottable : public KvRenderable
{
public:


private:
	std::string name_;
	QColor major_, minor_;
};
