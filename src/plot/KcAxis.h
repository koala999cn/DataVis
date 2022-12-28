#pragma once
#include <vector>
#include <string>
#include <memory>
#include "KvRenderable.h"
#include "KvTicker.h"
#include "KtColor.h"
#include "KtVector3.h"
#include "KpContext.h"
#include "KtMargins.h"
#include "layout/KvLayoutElement.h"


// �����ᣨ���ᣩʵ��
// ��������4��Ԫ�ع��ɣ�1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KcAxis : public KvRenderable, public KvLayoutElement
{
	using KvRenderable::float_t;
	using KvLayoutElement::rect_t;

public:
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using vec3 = KtVector3<float_t>;

	// 3d����ϵ�е�12��������
	enum KeType
	{
		k_near_left,
		k_near_right,
		k_near_bottom,
		k_near_top,

		k_far_left,
		k_far_right,
		k_far_bottom,
		k_far_top,

		k_floor_left,
		k_floor_right,
		k_ceil_left,
		k_ceil_right,

		k_left = k_near_left,
		k_right = k_near_right,
		k_bottom = k_near_bottom,
		k_top = k_near_top
	};

	enum KeTickOrient
	{
		k_x, k_neg_x, k_bi_x,
		k_y, k_neg_y, k_bi_y,
		k_z, k_neg_z, k_bi_z
	};

	struct KpTickContext : public KpPen
	{
		float length;

		KpPen& operator=(const KpPen& pen) {
			style = pen.style;
			width = pen.width;
			color = pen.color;
			return *this;
		}
	};

	KcAxis(KeType type, int dim, bool main);

	KeType type() const { return type_; }
	void setType(KeType t) { type_ = t; }

	const point3& start() const { return start_; }
	void setStart(const point3& v) { start_ = v; }
	void setStart(float_t x, float_t y, float_t z) { start_ = point3(x, y, z); }

	const point3& end() const { return end_; }
	void setEnd(const point3& v) { end_ = v; }
	void setEnd(float_t x, float_t y, float_t z) { end_ = point3(x, y, z); }

	void setExtent(const point3& st, const point3& ed) {
		start_ = st, end_ = ed;
	}

	const vec3& tickOrient() const { return tickOrient_; }
	vec3& tickOrient() { return tickOrient_; }

	const vec3& labelOrient() const { return labelOrient_; }
	vec3& labelOrient() { return labelOrient_; }

	bool tickBothSide() const { return tickBothSide_; }
	bool& tickBothSide() { return tickBothSide_; }

	/// range 

	float_t lower() const { return lower_; }
	float_t& lower() { return lower_; }
	float_t upper() const { return upper_; }
	float_t& upper() { return upper_; }

	void setRange(float_t l, float_t u) {
		lower_ = l, upper_ = u;
	}

	float_t length() const { 
		return upper() - lower(); 
	}

	bool showBaseline() const { return showBaseline_; }
	bool& showBaseline() { return showBaseline_; }

	bool showTick() const { return showTick_; }
	bool& showTick() { return showTick_; }

	bool showSubtick() const { return showSubtick_; }
	bool& showSubtick() { return showSubtick_; }

	bool showTitle() const { return showTitle_; }
	bool& showTitle() { return showTitle_; }

	bool showLabel() const { return showLabel_; }
	bool& showLabel() { return showLabel_; }

	void showAll() {
		showBaseline_ = true;
		showTick_ = true, showSubtick_ = true;
		showTitle_ = true, showLabel_ = true;
	}

	const std::string& title() const { return title_; }
	std::string& title() { return title_; }

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

	const KpPen& baselineContext() const { return baselineCxt_; }
	KpPen& baselineContext() { return baselineCxt_; }

	const KpTickContext& tickContext() const { return tickCxt_; }
	KpTickContext& tickContext() { return tickCxt_; }

	const KpTickContext& subtickContext() const { return subtickCxt_; }
	KpTickContext& subtickContext() { return subtickCxt_; }

	/// colors

	const color4f& titleColor() const { return titleColor_; }
	color4f& titleColor() { return titleColor_; }

	const color4f& labelColor() const { return labelColor_; }
	color4f& labelColor() { return labelColor_; }

	/// fonts

	//QFont labelFont() const { return labelFont_; }
	//void setLabelFont(QFont font) { labelFont_ = font; }

	//QFont titleFont() const { return titleFont_; }
	//void setTitleFont(QFont font) { titleFont_ = font; }

	std::shared_ptr<KvTicker> ticker() const;
	void setTicker(std::shared_ptr<KvTicker> tic);

	// NB������֮�󣨼�����calcSize֮�󣩣��ú������ܷ�����Чֵ
	aabb_t boundingBox() const override {
		return box_; 
	}

	void draw(KvPaint* paint) const override {
		return draw_(paint, false);
	}

	// ����tick����ֵ������tick���������ϵĵ�3ά���꣨�������꣩
	point3 tickPos(double val) const;

	// ���ص�ǰaxis����Ļ������ռ�ĳߴ磨���ش�С��
	KtMargins<float_t> calcMargins(KvPaint* paint) const;

	int dim() const { return dimReal_; }

	bool main() const { return main_; }

	bool inversed() const { return inv_; }
	void setInversed(bool inv) { inv_ = inv; }

	KeType typeReal() const; // ����swap������axis����ʵ��λ����

	// �ڲ�ʹ��
	void setSwapped_(int dimSwap) { dimSwapped_ = dimSwap; }

private:
	void draw_(KvPaint*, bool calcBox) const;
	void drawTicks_(KvPaint*, bool calcBox) const; // �������п̶�
	void drawTick_(KvPaint*, const point3& anchor, double length, bool calcBox) const; // ���Ƶ����̶��ߣ��������̶��븱�̶�
	void drawLabel_(KvPaint* paint, const std::string_view& label, const point3& anchor, bool calcBox) const;

	int labelAlignment_(KvPaint* paint, bool toggleTopBottom) const; // ����label��orientation�ж�label��alignment
	bool tickAndLabelInSameSide_() const; // �ж�tick��tick-label�Ƿ�λ���������ͬ��

	size_t calcSize_(void* cxt) const final;

	// ������3d�ռ�����ı������3��������topLeft, hDir, vDir
	void calcLabelPos_(KvPaint*, const std::string_view& label, const point3& anchor, point3& topLeft, point3& hDir, point3& vDir) const;

private:
	KeType type_;
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	float_t lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	KpPen baselineCxt_;
	KpTickContext tickCxt_, subtickCxt_;

	double labelPadding_{ 2 }; 
	double titlePadding_{ 2 };

	color4f labelColor_{ 0, 0, 0, 1 }, titleColor_{ 0, 0, 0, 1 };

	//QFont labelFont_, titleFont_;

	point3 start_, end_;
	vec3 tickOrient_;
	vec3 labelOrient_;
	bool tickBothSide_{ false };

	std::shared_ptr<KvTicker> ticker_;

	int dimReal_; // 0��ʾx�ᣬ1��ʾy�ᣬ2��ʾz�ᣬ-1��ʾ������?��������ʾcolorbar��
	int dimSwapped_; 
	bool main_{ true }; // �Ƿ���������
	bool inv_{ false }; // �Ƿ�ת������

	// ���³�Ա����label-box��layout��pose��TODO��
	
	// ��ʼ״̬�£�label-boxλ�ڿ̶����������ṹ�ɵ�ƽ����
	// 
	// label-box�Ĳ�������̬���������anchor�����
	// ��labelλ�����������࣬��anchor��λ��label-box��right-center
	// ��labelλ����������Ҳ࣬��anchor��λ��label-box��left-center
	// ��lebelλ����������²࣬��anchor��λ��label-box��top-center
	// ��lebelλ����������ϲ࣬��anchor��λ��label-box��bottom-center
	// �ڲ�����labelPadding_������£���label��tickͬ�࣬anchor����̶��ߵ�ĩ���غϣ�������̶ȵ��غ�
	
	// �ñ���ȷ��label-box�ĳ�ʼ����״̬
	bool labelVertical_{ false }; // ��true�� label�ı������У��ı�˳��+y/-z�᷽����չ
	                              // ��false��lebel�ı������У��ı�˳��+x�᷽����չ

	// �ñ���ȷ��label-box����ڳ�ʼ���ֵ���̬�仯
	point3 pose_{ 0, 0, 0 }; // pose_[0]��ʾƫ��yaw��������label-boxƽ��Ĵ��ߣ���anchor�㣩����ת�Ƕ�
	                         // pose_[1]��ʾ����pitch��������ƽ�����������ֱ�ߣ���anchor�㣩����ת�Ƕ�
	                         // pose_[2]��ʾ����roll��������������Ĵ��ߣ���anchor�㣩����ת�Ƕ�


	// ����Ϊ�ߴ���㻺�����ʱ����
	std::vector<point2> labelSize_;
	mutable point2 titleSize_;
	mutable point3 titleAnchor_; // title�ı����top-left����
	vec3 hDir_, vDir_; // label�ı���ˮƽ�ʹ�ֱ��չ����
	mutable aabb_t box_; // Ԥ�����aabb
};
