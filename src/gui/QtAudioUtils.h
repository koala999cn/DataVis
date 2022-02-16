#pragma once
#include <QString>

class QWidget;

class  QtAudioUtils
{
public:
	static QString getSavePath(QWidget* parent = nullptr);
	static QString getOpenPath(QWidget* parent = nullptr);

private:
	QtAudioUtils() {}
	~QtAudioUtils() {}
};

