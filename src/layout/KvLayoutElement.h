#pragma once
#include "KtAABB.h"
#include "KeAlignment.h"


//
// ����Ԫ�س�����
// ÿ������Ԫ������߿�(outterRect)���ڱ߿�(innerRect)�ͻ�������(content)����
// Ԫ�����ݵĻ����������ڱ߿�(����clipRect)����߿���ڱ߿�ļ����marginsȷ��
// margins���û�����, contentSize�ɼ̳������
// outterRect, innerRect��layout�����㶨
// 
// Ŀǰ��֧���������͵�Ԫ�أ�
// һ�ǹ̶��ߴ磬��ʱcalcSize���ط���ֵ��layout���澡�����óߴ�ΪԪ�ط���ռ�
// ���Ǹ����ߴ磬��ʱcalcSize������ֵ��layout���潫ʣ��ռ䰴��extraShares���صķݶ�ΪԪ�ط���ռ�
// 

class KvLayoutElement
{
public:
	using float_t = double;
	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	KvLayoutElement() = default;
	KvLayoutElement(KvLayoutElement* parent) : parent_(parent) {}

	KeAlignment align() const { return align_; }
	KeAlignment& align() { return align_; }

	KvLayoutElement* parent() { return parent_; }
	void setParent(KvLayoutElement* p) { parent_ = p; }

	// ����arrangeϵ�к���֮ǰ�����ȵ���calcSize

	virtual void arrange_(int dim, float_t lower, float_t upper);

	void arrange(const rect_t& rc) {
		for (int i = 0; i < 2; i++)
			arrange_(i, rc.lower()[i], rc.upper()[i]);
	}

	margins_t& margins() { return margins_; }
	const margins_t& margins() const { return margins_; }

	void setMargins(float l, float t, float r, float b);

	const rect_t& outterRect() const { return oRect_; }

	const rect_t& innerRect() const { return iRect_; }

	const size_t& contentSize() const { return contentSize_; }


	// ����content�ĳߴ磬���������iRect_��
	// iRect_��lower�����margins_��lower��iRect_��size����contentSize
	// ���ط�0����̶��ߴ磬����0��ʾ������ߴ磬��layout������ݿ��ÿռ����
	void calcSize(void* cxt) const { 
		contentSize_ = calcSize_(cxt);
	}

	// ���������Ŀռ��С�����ڲ�ʹ��
	// ��calcSize֮��arrange֮ǰ����
	size_t expectRoom() const {	
		size_t room(0); 
		if (contentSize().x() > 0) 
			room.x() = contentSize_.x() + margins_.lower().x() + margins_.upper().x();
		if (contentSize().y() > 0)
			room.y() = contentSize_.y() + margins_.lower().y() + margins_.upper().y();
		return room;
	}

	// ����x/yά����Ҫ�������ռ�ķݶ�
	virtual point2i extraShares() const {
		return { contentSize_.x() == 0 ? shareFactor_.x() : 0,
			contentSize_.y() == 0 ? shareFactor_.y()  : 0 };
	}

	const point2i& shareFactor() const { return shareFactor_; }
	void setShareFactor(const point2i& f) { shareFactor_ = f; }

protected:
	virtual size_t calcSize_(void* cxt) const = 0;

protected:
	mutable size_t contentSize_{ 0 };
	rect_t iRect_{ point_t(0) ,point_t(0) }, oRect_{ point_t(0) ,point_t(0) }; // �ڡ���߿���߿����ڲ��֣��ڱ߿����ڻ���
	margins_t margins_{ point_t(0) ,point_t(0) }; // ����߿������ף�ȱʡ������
	KvLayoutElement* parent_{ nullptr };
	point2i shareFactor_{ 1, 1 }; // �ݶ����ӣ�ֵԽ�����Ŀռ�Խ��
	KeAlignment align_{ 0 }; // ���뷽ʽ��������Ŀռ����contentSizeʱ���ñ�Ǿ�������iRect�ķ�ʽ
};
