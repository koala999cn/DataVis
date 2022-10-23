#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>


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

	void onDoubleClicked() override;

protected:

	// һ������Դ���Դ������KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) = 0;

	void showThemeProperty_();

	// ��Ӧ��theme��ʱ��ͬ��������canvas, layout, palette������ѡ��
	void applyTheme_(const std::string& name, KvThemedPlot* plot);

	// ����plottable�İ��������������ṩsplitChannels_�޹صĽӿ�
	// �û���createPlottable_�ӿ�ʵ���пɵ���
	template<typename PLT_TYPE>
	std::vector<KvPlottable*> createPlts_(KvDataProvider* prov) {
		if (prov->channels() == 1)
			return { new PLT_TYPE(prov->name()) };

		std::vector<KvPlottable*> plts;
		plts.resize(prov->channels());

		for (kIndex ch = 0; ch < prov->channels(); ch++) {
			std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
			plts[ch] = new PLT_TYPE(name);
		}

		return plts;
	}

	void updateTheme_();

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // �˿�id��plottable���е�ӳ��
												 // 1���˿ڿ����ж��ͨ����Ϊ�˿���ӳ�䵽���plottable��ÿ��ͨ����Ӧ1��plottable��
	
	// ����stream���룬����1�����ݸ���. ��Ϊ���������Ƕ�̬�ģ���ͼ��ʱ�����ֶ���
	std::map<KcPortNode*, std::shared_ptr<KvData>> streamData_;

	std::string themeName_, canvasName_, layoutName_, paletteName_;

	bool splitChannels_{ false }; // ��ͨ�������Ǵ���1�����Ƕ��plt�� 
};
