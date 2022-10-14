#pragma once
#include "KtColor.h"
#include <functional>
#include "KvRenderable.h"
#include "KpContext.h"


enum KeAlignment
{
	k_align_center = 0x00,
	k_align_left = 0x01,
	k_align_right = 0x02,
	k_align_top = 0x04,
	k_align_bottom = 0x08,

	// ��λ�ڱ߿��������ʱ��ʹ������2��ö��������ˮƽ���������ȼ�
	// ���磬���k_align_left��k_align_top�������ã���
	//   -- ��k_align_vert_first��Чʱ��λ�ڱ߿�Ķ���λ�ÿ�����룻
	//   -- ��k_align_horz_first��Чʱ��λ�ڱ߿�����λ�ÿ��϶��롣
	k_align_vert_first = 0x40,
	k_align_horz_first = 0x80
};

// һ���򵥵�3d���ƽӿ�

class KvPaint
{
public:
	using color_t = color4f;
	using float_type = typename KvRenderable::float_type;
	using point = KtPoint<float_type, 3>;
	using point_getter = std::function<point(unsigned)>;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(double size) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void drawPoint(const point& pt) = 0;

	virtual void drawPoints(const point pts[], unsigned count);

	void drawPoints(point_getter fn, unsigned count);

	virtual void drawLine(const point& from, const point& to) = 0;

	virtual void drawLineStrip(const point pts[], unsigned count);

	void drawLineStrip(point_getter fn, unsigned count);

	virtual void drawLineLoop(const point pts[], unsigned count);

	void drawLineLoop(point_getter fn, unsigned count);

	virtual void drawText(const point& anchor, const char* text, int align) = 0;


	// һЩ��ݺ���

	void apply(const KpLineContext& cxt);

	void apply(const KpScatterContext& cxt);

private:

};
