#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>

class KcImPlot3d;
class KvPlottable;

class KcRdPlot3d : public KvDataRender
{
public:

	using super_ = KvDataRender;

	KcRdPlot3d();
	virtual ~KcRdPlot3d();

	// �������빹��plottables
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	void onDelLink(KcPortNode* from, KcPortNode* to) final;

	bool onStartPipeline() final;

	void onInput(KcPortNode* outPort, unsigned inPort) final;

	void output() final {}

	void showProperySet() final;

private:
	std::shared_ptr<KcImPlot3d> plot3d_;
	std::multimap<int, KvPlottable*> port2Plts_; // �˿�id��plottable���е�ӳ��
												 // 1���˿ڿ����ж��ͨ����Ϊ�˿���ӳ�䵽���plottable��ÿ��ͨ����Ӧ1��plottable��
};

