#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>
#include <vector>


class KvPlot;
class KvPlottable;
class KvDataProvider;
class KvThemedPlot;

class KvRdPlot : public KvDataRender
{
public:

	using super_ = KvDataRender;

	KvRdPlot(const std::string_view& name, const std::shared_ptr<KvPlot>& plot);
	virtual ~KvRdPlot();

	// �������빹��plottables
	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	void onDelLink(KcPortNode* from, KcPortNode* to) override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	void output() override {}

	void showProperySet() override;


protected:

	// һ������Դ���Դ������KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) = 0;

	void showThemeProperty_();

	// ��Ӧ��theme��ʱ��ͬ��������canvas, layout, palette������ѡ��
	void applyTheme_(const std::string& name, KvThemedPlot* plot);

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // �˿�id��plottable���е�ӳ��
												 // 1���˿ڿ����ж��ͨ����Ϊ�˿���ӳ�䵽���plottable��ÿ��ͨ����Ӧ1��plottable��
	
	std::string themeName_, canvasName_, layoutName_, paletteName_;
};
