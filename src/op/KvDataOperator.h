#pragma once
#include "prov/KvDataProvider.h"
#include <vector>


// 数据处理算子的抽象类

class KvDataOperator : public KvDataProvider
{
	using super_ = KvDataProvider;

public:

	KvDataOperator(const std::string_view& name);

	/// node接口

	// 默认输入输出端口均为1
	unsigned inPorts() const override { return 1; }

	// 主要前期fetchData，解决输出数据规范的问题（provider接口方法）
	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	void onDelLink(KcPortNode* from, KcPortNode* to) override;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	// 作一些一致性检测工作
	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	/// provider接口

	std::shared_ptr<KvData> fetchData(kIndex outPort) const override;

	void notifyChanged(unsigned outPort = -1) override;

	unsigned dataStamp(kIndex outPort) const override;

	// 获取数据规格的方法：若output有效，优先从output获取；否则从inputs_获取
	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const override;

	kReal step(kIndex outPort, kIndex axis) const override;

	kIndex size(kIndex outPort, kIndex axis) const override;

	void output() override;


	/// 自定义接口

	// inPort输入端口是否接受dataSpec规格的数据 ？ 
	virtual bool permitInput(int dataSpec, unsigned inPort) const = 0;

	// 当前语义：op的配置参数发生变化，须重构处理器并重新生成输出
	void setOutputExpired(unsigned outPort);
	bool isOutputExpired(unsigned outPort) const;
	bool isOutputExpired() const; // 任意输出端口过期，返回true

	bool isInputUpdated(unsigned inPort) const;
	bool isInputUpdated() const; // // 任意输入端口更新，返回true

protected:

	// 该方法用于同步配置参数和输入数据规格的变化
	// NB: 目前假定管线运行期间，输入数据的维度不会发生变化
	virtual void prepareOutput_() = 0; 

	// 该函数不用考虑配置参数和输入数据规格的变化，也不同考虑时间戳的比对与更新
	virtual void outputImpl_() = 0;

	/// 几个获取输入数据规格的帮助函数

	// 特定输入端口
	int inputSpec_(kIndex inPort) const;
	kRange inputRange_(kIndex inPort, kIndex axis) const;
	kReal inputStep_(kIndex inPort, kIndex axis) const;
	kIndex inputSize_(kIndex inPort, kIndex axis) const;

	// 首个非空输入端口
	int inputSpec_() const;
	kRange inputRange_(kIndex axis) const;
	kReal inputStep_(kIndex axis) const;
	kIndex inputSize_(kIndex axis) const;

protected:
	std::vector<std::shared_ptr<KvData>> idata_;
	std::vector<std::shared_ptr<KvData>> odata_;
	std::vector<unsigned> idataStamps_;
	std::vector<unsigned> odataStamps_;
	std::vector<bool> outputExpired_;
	std::vector<KcPortNode*> inputs_;
};

