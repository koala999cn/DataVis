#pragma once
#include "KvImWindow.h"


// ģ̬���ڻ���
class KvImModalWindow : public KvImWindow
{
public:
	KvImModalWindow(const std::string_view& name);
	virtual ~KvImModalWindow();

	void update() override;


protected:

	// �򿪸�ģ̬popup��ֻ�д���popup��ģ̬��̨��update
	// �û�����ʾ���ã�update����Ҳ���ڱ�Ҫ��ʱ����øú���
	void open_();

	bool opened_() const;

	// �رո�ģ̬popup
	// ����EndPopup֮ǰ���ã������û���updateImpl_ʵ���е���
	void close_();
};


