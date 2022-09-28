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

	auto& cleanData() { return cleanData_; }

private:
	bool loadData_(); // load raw data from filepath_ to rawData_

private:
	const std::string& filepath_;  // �����ļ�·��
	std::unique_ptr<KcImTextCleanWindow> cleanWindow_;
	std::vector<std::vector<std::string>> rawData_; // ��filepath_��ȡ���ַ�������
	std::vector<std::vector<double>> cleanData_; // ��ϴ�������
};
