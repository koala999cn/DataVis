#pragma once
#include <vector>
#include <string>
#include <memory>
#include "KvRenderable.h"
#include "KvTicker.h"
#include "KtColor.h"
#include "KtVector3.h"
#include "KpContext.h"


// �����ᣨ���ᣩʵ��
// ��������4��Ԫ�ع��ɣ�1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KcAxis : public KvRenderable
{
public:

	using point3 = KtPoint<float_type, 3>;
	using vec3 = KtVector3<float_type>;

	enum KeTickOrient
	{
		k_x, k_neg_x, k_bi_x,
		k_y, k_neg_y, k_bi_y,
		k_z, k_neg_z, k_bi_z
	};

	struct KpTickContext : public KpLineContext
	{
		double length;
	};

	KcAxis();

	const point3& start() const { return start_; }
	void setStart(const point3& v) { start_ = v; }
	void setStart(double x, double y, double z) { start_ = point3(x, y, z); }

	const point3& end() const { return end_; }
	void setEnd(const point3& v) { end_ = v; }
	void setEnd(double x, double y, double z) { end_ = point3(x, y, z); }

	const vec3& tickOrient() const { return tickOrient_; }
	vec3& tickOrient() { return tickOrient_; }

	bool tickShowBothSide() const { return tickShowBothSide_; }
	bool& tickShowBothSide() { return tickShowBothSide_; }

	/// range 

	double lower() const { return lower_; }
	double upper() const { return upper_; }
	std::pair<double, double> range() const { return { lower_, upper_ }; }
	void setLower(double lower) { lower_ = lower; }
	void setUpper(double upper) { upper_ = upper; }
	void setRange(double lower, double upper) { lower_ = lower, upper_ = upper; }

	double length() const {
		return upper_ - lower_; // == (end - start).length ? 
	}

	bool showBaseline() const { return showBaseline_; }
	void setShowBaseline(bool b) { showBaseline_ = b; }

	bool showTick() const { return showTick_; }
	void setShowTick(bool b) { showTick_ = b; }

	bool showSubtick() const { return showSubtick_; }
	void setShowSubtick(bool b) { showSubtick_ = b; }

	bool showTitle() const { return showTitle_; }
	void setShowTitle(bool b) { showTitle_ = b; }

	bool showLabel() const { return showLabel_; }
	void setShowLabel(bool b) { showLabel_ = b; }

	void showAll() {
		showBaseline_ = true;
		showTick_ = true, showSubtick_ = true;
		showTitle_ = true, showLabel_ = true;
	}

	const std::string& title() const { return title_; }
	void setTitle(const std::string& title) { title_ = title; }

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

	const KpLineContext& baselineContext() const { return baselineCxt_; }
	KpLineContext& baselineContext() { return baselineCxt_; }

	const KpTickContext& tickContext() const { return tickCxt_; }
	KpTickContext& tickContext() { return tickCxt_; }

	const KpTickContext& subtickContext() const { return subtickCxt_; }
	KpTickContext& subtickContext() { return subtickCxt_; }

	// tick������rlenΪ�ο�ֵȡ�ٷ���
	// �磬��tickΪ5��rlenΪ300�������tick��ʵ�ʳ���Ϊ300*5%����15
	// rlenͨ����Ϊ����ϵaabb�ĶԽ��߳���
	void setRefLength(double rlen) { refLength_ = rlen; }

	/// colors

	color4f titleColor() const { return titleColor_; }
	void setTitleColor(color4f clr) { titleColor_ = clr; }

	color4f labelColor() const { return labelColor_; }
	void setLabelColor(color4f clr) { labelColor_ = clr; }

	/// fonts

	//QFont labelFont() const { return labelFont_; }
	//void setLabelFont(QFont font) { labelFont_ = font; }

	//QFont titleFont() const { return titleFont_; }
	//void setTitleFont(QFont font) { titleFont_ = font; }

	std::shared_ptr<KvTicker> ticker() const;
	void setTicker(std::shared_ptr<KvTicker> tic);

	// ����tick����ֵ������tick���������ϵĵ�3ά����
	point3 tickPos(double val) const;
	aabb_type boundingBox() const override;

	void draw(KvPaint*) const override;


	// ������������ɾ��

	void setTickOrient(const vec3& v, bool bothSide) {
		tickOrient_ = v;
		tickShowBothSide_ = bothSide;
	}

	void setTickOrient(KeTickOrient orient);


private:
	void drawTicks_(KvPaint*) const; // �������п̶�
	void drawTick_(KvPaint*, const point3& anchor, double length) const; // ���Ƶ����̶��ߣ��������̶��븱�̶�
	static int labelAlignment_(const vec3& orient); // ����tick��orientation�ж�label��alignment

private:
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	double lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	KpLineContext baselineCxt_;
	KpTickContext tickCxt_, subtickCxt_;

	double labelPadding_; // �̶�label����̶��ߵľ���
	mutable double refLength_; // ��׼�ο�����. tickLength_, subtickLength_, labelPadding_��Ϊ�����ֵ
	                           // һ��ȡAABB�ĶԽ��߳���
	color4f labelColor_{ 0, 0, 0, 1 }, titleColor_{ 0, 0, 0, 1 };

	//QFont labelFont_, titleFont_;

	point3 start_, end_;
	vec3 tickOrient_;
	bool tickShowBothSide_;

	std::shared_ptr<KvTicker> ticker_;
};
