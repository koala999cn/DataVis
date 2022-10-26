#pragma once
#include "KvAction.h"
#include <memory>
#include <vector>
#include <string_view>

class KcImTextCleaner;

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
	std::string text_; // ���ִ���ʽ�ݴ���ļ�����
	std::unique_ptr<KcImTextCleaner> cleanWindow_;
	std::vector<std::vector<std::string_view>> rawData_; // ��filepath_��ȡ���ַ�������
	std::vector<std::vector<double>> cleanData_; // ��ϴ�������
};
