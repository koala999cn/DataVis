#pragma once
#include "KvDataRender.h"
#include <memory>
#include <map>


class KvPlot;
class KcAxis;
class KcCoordPlane;
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

	void showPropertySet() override;

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
		if (prov->channels(port->index()) == 1 || !splitChannels_)
			return { new PLT_TYPE(prov->name()) };

		std::vector<KvPlottable*> plts;
		plts.resize(prov->channels(port->index()));

		for (kIndex ch = 0; ch < prov->channels(port->index()); ch++) {
			std::string name = prov->name() + " - ch" + std::to_string(ch);
			plts[ch] = new PLT_TYPE(name);
		}

		return plts;
	}


	/// ������ʾ���ӽӿ�

	virtual void showPlotProperty_();

	virtual void showThemeProperty_();

	virtual void showCoordProperty_();

	    virtual void showAxisProperty_(KcAxis&);

		virtual void showPlaneProperty_(KcCoordPlane&);

	virtual void showLegendProperty_();
	
	virtual void showColorBarProperty_();

	virtual void showPlottableProperty_();

	    // ����true��ʾ�û�������type
	    bool showPlottableBasicProperty_(unsigned idx, KvPlottable*);

		void showPlottableArrangeProperty_(KvPlottable*);

		void showPlottableColoringProperty_(KvPlottable*);

		void showPlottableDefaultZProperty_(KvPlottable*);

	    virtual void showPlottableSpecificProperty_(KvPlottable*) {};

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

	using data_ptr = std::shared_ptr<KvData>;
	static data_ptr streaming_(data_ptr curData, data_ptr newData, unsigned nx);

	// ����inputs��range���Զ�����plot��������extend
	void autoRange_();

protected:
	std::shared_ptr<KvPlot> plot_;
	std::multimap<int, KvPlottable*> port2Plts_; // �˿�id��plottable���е�ӳ��
												 // 1���˿ڿ����ж��ͨ����Ϊ�˿���ӳ�䵽���plottable��ÿ��ͨ����Ӧ1��plottable��
	
	// ����stream���룬����1�����ݸ���. ��Ϊ���������Ƕ�̬�ģ���ͼ��ʱ�����ֶ���
	std::map<KcPortNode*, std::shared_ptr<KvData>> streamData_;

	std::pair<std::string, std::string> curTheme_[4];

	bool splitChannels_{ false }; // ��ͨ�������Ǵ���1�����Ƕ��plt�� 

};
