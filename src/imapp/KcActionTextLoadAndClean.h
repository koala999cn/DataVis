#pragma once
#include "KvAction.h"
#include <memory>
#include <vector>

class KcImTextCleanWindow;

class KcActionTextLoadAndClean : public KvAction
{
public:
	KcActionTextLoadAndClean(const std::string& filepath);

	bool trigger() override;

	void update() override;

private:
	bool loadData_(); // load raw data from filepath_ to rawData_

private:
	const std::string& filepath_;  // �����ļ�·��
	std::unique_ptr<KcImTextCleanWindow> dataView_;
	std::vector<std::vector<std::string>> rawData_; // ��filepath_��ȡ���ַ�������
};
