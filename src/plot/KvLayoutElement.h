#pragma once
#include "KtAABB.h"
#include "KtMargins.h"


//
// ����Ԫ�س�����
// ÿ������Ԫ������߿�(outterRect)���ڱ߿�(innerRect)�ͻ�������(content)����
// Ԫ�����ݵĻ����������ڱ߿�(����clipRect)����߿���ڱ߿�ļ����marginsȷ��
// margins���û�����
// outterRect, innerRect��layout����Э���趨
// contentSize�ɼ̳������
//

class KvLayoutElement
{
	using float_t = double;

public:

	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using margins_t = rect_t;
	using point_t = typename rect_t::point_t;

	constexpr static float_t null_size{ 0 };
	constexpr static float_t auto_fit_size{ -1 };

	
	void setMargins(const margins_t& m) { margins_ = m; }
	const margins_t& margins() const { return margins_; }

	
	virtual void setOutterRect(const rect_t& rc) {
		oRect_ = iRect_ = rc;
		iRect_.shrink(margins_);
	}

	rect_t outterRect() const { return oRect_; }

	rect_t innerRect() const { return iRect_; }

	
	size_t calcContentSize() const {
		return contentSize_ = calcContentSize_();
	}

	const size_t& contentSize() const { return contentSize_; }

	// ���������Ŀռ��С
	virtual size_t expectRoom() const {
		return contentSize_ + margins_.lower() + margins_.upper();
	}

protected:
	virtual size_t calcContentSize_() const = 0;

private:
	mutable size_t contentSize_; // �����Ԫ�����ݳߴ�
	rect_t iRect_, oRect_; // �ڡ���߿�
	margins_t margins_{ point_t(0) ,point_t(0) }; // ����߿������ף�ȱʡ������
};
