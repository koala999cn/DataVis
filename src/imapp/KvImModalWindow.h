#pragma once
#include "KvImWindow.h"


// ģ̬���ڻ���
class KvImModalWindow : public KvImWindow
{
public:
	KvImModalWindow(const std::string_view& name);
	KvImModalWindow(std::string&& name);
	virtual ~KvImModalWindow();

	void update() override;

	bool opened() const;

	// �򿪸�ģ̬popup��ֻ�д���popup��ģ̬��̨��update
	void open();

	// �رո�ģ̬popup
	void close();
};


