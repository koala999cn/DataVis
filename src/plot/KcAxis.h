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

	enum KeTextLayout
	{
		k_horz_top, // ��׼��壬����ͷ�����ϲ࣬�ײ����²�
		k_horz_bottom, // ���µ��õĺ�棬����ͷ�����²࣬�ײ����ϲ�
		k_vert_left, // ���棬����ͷ������࣬�ײ����Ҳ�
		k_vert_right // ���棬����ͷ�����Ҳ࣬�ײ������
	};

	enum KeTickSide
	{
		k_inside, k_outside, k_bothside
	};

	struct KpTickContext : public KpPen
	{
		float length{ 5 };
		KeTickSide side{ k_outside };
		float yaw{ 0 }; // ����������tick�Ĵ�ֱ�ߵ���ת�Ƕȣ����ȣ�
		float pitch{ 0 }; // ���ڻ�׼λ�ã������������ת�Ƕȣ����ȣ�

		KpPen& operator=(const KpPen& pen) {
			style = pen.style;
			width = pen.width;
			color = pen.color;
			return *this;
		}
	};

	struct KpTextContext
	{
		KpFont font;
		color4f color{ 0, 0, 0, 1 };
		KeTextLayout layout{ k_horz_top };
		bool billboard{ true }; // �Ƿ��Թ�����ģʽ��ʾtext����true��text��hDirʼ�ճ�����Ļ���Ҳ࣬vDirʼ�ճ�����Ļ���²�
		float yaw{ 0 }; // ��text-boxƽ��Ĵ��ߣ������ĵ㣩����ת�Ƕȣ����ȣ�
		float pitch{ 0 }; // ��hDir��vDir����ת�Ƕȣ����ȣ�
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

	const KpPen& baselineContext() const { return baselineCxt_; }
	KpPen& baselineContext() { return baselineCxt_; }

	std::shared_ptr<KvTicker> ticker() const;
	void setTicker(std::shared_ptr<KvTicker> tic);

	const KpTickContext& tickContext() const { return tickCxt_; }
	KpTickContext& tickContext() { return tickCxt_; }

	const KpTickContext& subtickContext() const { return subtickCxt_; }
	KpTickContext& subtickContext() { return subtickCxt_; }

	/// title properties

	const std::string& title() const { return title_; }
	std::string& title() { return title_; }

	float titlePadding() const { return titlePadding_; }
	float& titlePadding() { return titlePadding_; }

	KpTextContext titleContext() const { return titleCxt_; }
	KpTextContext& titleContext() { return titleCxt_; }

	/// label properties

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

	float labelPadding() const { return labelPadding_; }
	float& labelPadding() { return labelPadding_; }

	KpTextContext labelContext() const { return labelCxt_; }
	KpTextContext& labelContext() { return labelCxt_; }


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
	size_t calcSize_(void* cxt) const final;

	void draw_(KvPaint*, bool calcBox) const;
	void drawTicks_(KvPaint*, bool calcBox) const; // �������п̶�
	void drawTick_(KvPaint*, const point3& anchor, double length, bool calcBox) const; // ���Ƶ����̶��ߣ��������̶��븱�̶�
	void drawText_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, const point3& anchor, bool calcBox) const;

	int labelAlignment_(KvPaint* paint, bool toggleTopBottom) const; // ����label��orientation�ж�label��alignment
	bool tickAndLabelInSameSide_() const; // �ж�tick��tick-label�Ƿ�λ���������ͬ��

	// ����tick�ĳ���
	vec3 calcTickOrient_(KvPaint*) const;

	point3 calcTitleAnchor_(KvPaint*) const;

	// ������3d�ռ�����ı������3��������topLeft, hDir, vDir
	void calcTextPos_(KvPaint*, const std::string_view& label, const KpTextContext& cxt, 
		const point3& anchor, point3& topLeft, vec3& hDir, vec3& vDir) const;

	// ����layout����topLeft��hDir & vDir
	static void fixTextLayout_(KeTextLayout lay, const size_t& textBox, point3& topLeft, vec3& hDir, vec3& vDir);

private:
	KeType type_;
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	float_t lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	KpPen baselineCxt_;
	KpTickContext tickCxt_, subtickCxt_;

	// label��������
	// 
	// ��ʼ״̬�£�label-boxλ�ڿ̶����������ṹ�ɵ�ƽ����
	// 
	// label-box�Ĳ�������̬���������anchor�����
	// ��labelλ�����������࣬��anchor��λ��label-box��right-center
	// ��labelλ����������Ҳ࣬��anchor��λ��label-box��left-center
	// ��lebelλ����������²࣬��anchor��λ��label-box��top-center
	// ��lebelλ����������ϲ࣬��anchor��λ��label-box��bottom-center
	// �ڲ�����labelPadding_������£���label��tickͬ�࣬anchor����̶��ߵ�ĩ���غϣ�������̶ȵ��غ�

	float labelPadding_{ 2 }; 
	KpTextContext labelCxt_;

	// title��������

	float titlePadding_{ 2 };
	KpTextContext titleCxt_;

	point3 start_, end_;

	std::shared_ptr<KvTicker> ticker_;

	int dimReal_; // 0��ʾx�ᣬ1��ʾy�ᣬ2��ʾz�ᣬ-1��ʾ�����뽻���������ᣨ������ʾcolorbar��
	int dimSwapped_; // -1��ʾ��������ά�ȵ������ύ��
	bool main_{ true }; // �Ƿ���������
	bool inv_{ false }; // �Ƿ�ת������


	// ����Ϊ�ߴ���㻺�����ʱ����
	//
	mutable vec3 tickOrient_;
	mutable vec3 labelOrient_;
	std::vector<point2> labelSize_;
	mutable point2 titleSize_;
	mutable point3 titleAnchor_; // title�ı����top-left����
	vec3 hDir_, vDir_; // label�ı���ˮƽ�ʹ�ֱ��չ����
	mutable aabb_t box_; // Ԥ�����aabb
};
