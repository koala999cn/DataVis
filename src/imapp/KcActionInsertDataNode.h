#pragma once
#include "KvAction.h"
#include <memory>
#include <vector>

class KcImDataMaker;
class KvData;

class KcActionInsertDataNode : public KvAction
{
public:
	using matrixd = std::vector<std::vector<double>>;

	KcActionInsertDataNode(const std::string& filepath, const matrixd& idata);

	bool trigger() override;

	void update() override;

private:
	std::unique_ptr<KcImDataMaker> dataMaker_;
	const std::string& filepath_;
	const matrixd& idata_;
	std::shared_ptr<KvData> odata_;
};
