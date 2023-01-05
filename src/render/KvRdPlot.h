#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>
#include "KuStrUtil.h"

class KvPlot;
class KcAxis;
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

	void showProperySet() override;

	void onDoubleClicked() override;

	virtual void onThemeChanged(int type);

protected:

	// һ������Դ���Դ������KvPlottable
	virtual std::vector<KvPlottable*> createPlottable_(KcPortNode* port) = 0;
	
	// ����plottable�İ��������������ṩsplitChannels_�޹صĽӿ�
	// �û���createPlottable_�ӿ�ʵ���пɵ���
	template<typename PLT_TYPE>
	std::vector<KvPlottable*> createPlts_(KcPortNode* port) {
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());
		if (prov->channels(port->index()) == 1)
			return { new PLT_TYPE(prov->name()) };

		std::vector<KvPlottable*> plts;
		plts.resize(prov->channels(port->index()));

		for (kIndex ch = 0; ch < prov->channels(port->index()); ch++) {
			std::string name = prov->name() + " - ch" + KuStrUtil::toString(ch);
			plts[ch] = new PLT_TYPE(name);
		}

		return plts;
	}


	/// ������ʾ���ӽӿ�

	virtual void showPlotProperty_();

	virtual void showThemeProperty_();

	virtual void showCoordProperty_();

	    virtual void showAxisProperty_(KcAxis&);

	virtual void showLegendProperty_();
	
	virtual void showColorBarProperty_();

	virtual void showPlottableProperty_();

	    void showPlottableTypeProperty_(unsigned idx);

		void showPlottableSampCountProperty_(unsigned idx);

	    virtual void showPlottableSpecificProperty_(unsigned idx) {};

	////////////////////////////////////////////////////


	/// plottable����ת����ؽӿڣ���showPlottableTypeProperty_��������

	// ����֧�ֵ�plottable������
	virtual unsigned supportPlottableTypes_() const = 0;

	// ����plt������
	virtual int plottableType_(KvPlottable* plt) const = 0;

	// ���ص�iType���͵��ַ���
	virtual const char* plottableTypeStr_(int iType) const = 0;

	// ������iType���͵�plottable
	virtual KvPlottable* newPlottable_(int iType, const std::string& name) = 0;

	// ��iType���͵�plottable�Ƿ�֧��KvData����
	virtual bool plottableMatchData_(int iType, const KvData& d) const { return true; }

	////////////////////////////////////////////////////


	void updateTheme_();

	static void streaming_(std::shared_ptr<KvData> curData, std::shared_ptr<KvData> newData, double xrange);

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // �˿�id��plottable���е�ӳ��
												 // 1���˿ڿ����ж��ͨ����Ϊ�˿���ӳ�䵽���plottable��ÿ��ͨ����Ӧ1��plottable��
	
	// ����stream���룬����1�����ݸ���. ��Ϊ���������Ƕ�̬�ģ���ͼ��ʱ�����ֶ���
	std::map<KcPortNode*, std::shared_ptr<KvData>> streamData_;

	std::pair<std::string, std::string> curTheme_[4];

	bool splitChannels_{ true }; // ��ͨ�������Ǵ���1�����Ƕ��plt�� 

};
