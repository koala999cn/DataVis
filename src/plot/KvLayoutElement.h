#pragma once
#include "KtAABB.h"
#include "KtMargins.h"


//
// ����Ԫ�س�����
// ÿ������Ԫ������߿�(outterRect)���ڱ߿�(innerRect)�ͻ�������(content)����
// Ԫ�����ݵĻ����������ڱ߿�(����clipRect)����߿���ڱ߿�ļ����marginsȷ��
// margins���û�����
// outterRect, innerRect��layout�����㶨
//

class KvLayoutElement
{
	using float_t = double;

public:

	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	// ���øú���֮ǰ�����ȵ���calcSize
	virtual void arrange(const rect_t& rc) {
		if (rc.width()) arrange_(rc, 0);
		if (rc.height()) arrange_(rc, 1);
	}

	void setMargins(const margins_t& m) { margins_ = m; }
	const margins_t& margins() const { return margins_; }

	rect_t outterRect() const { return oRect_; }

	rect_t innerRect() const { return iRect_; }

	// ����content�ĳߴ磬���������iRect_��
	// iRect_��lower�����margins_��lower��iRect_��size����contentSize
	// ���ط�0����̶��ߴ磬����0��ʾ������ߴ磬��layout������ݿ��ÿռ����
	void calcSize() { 
		iRect_.lower() = margins_.lower();
		iRect_.upper() = iRect_.lower() + calcSize_();
	}

	// ���������Ŀռ��С�����ڲ�ʹ��
	// ��calcSize֮��arrange֮ǰ����
	size_t expectRoom() const {	
		auto room = iRect_.upper() + margins_.upper(); // iRect_.upper����margins_.lower + contentSize
		if (iRect_.width() == 0) room.x() = 0;
		if (iRect_.height() == 0) room.y() = 0;
		return room;
	}

	// ����x/yά����Ҫ�����Զ�����ռ������
	virtual point2i squeezeNeeded() const {
		return { iRect_.width() > 0, iRect_.height() > 0 };
	}

protected:
	virtual size_t calcSize_() const = 0;

	// ����iRect_�����contentSize
	// ���ô˺���֮��iRect_�ĳߴ罫���ٵ���contentSize
	float_t arrange_(const rect_t& rc, int dim);

protected:
	rect_t iRect_, oRect_; // �ڡ���߿�
	margins_t margins_{ point_t(0) ,point_t(0) }; // ����߿������ף�ȱʡ������

};
