#include "KuOglUtil.h"
#include "plot/KpContext.h"
#include "glad.h"


namespace kPrivate
{
	static int lineStipple(int lineStyle)
	{
		switch (lineStyle)
		{
		case KpPen::k_dot:   return 0xAAAA;
		case KpPen::k_dash:  return 0xCCCC;
		case KpPen::k_dash4: return 0xF0F0;
		case KpPen::k_dash8: return 0xFF00;
		case KpPen::k_dash_dot: return 0xF840;
		case KpPen::k_dash_dot_dot: return 0xF888;
		}
		return 0xFFFF;
	}
}


void KuOglUtil::glLineStyle(int style)
{
	if (style == KpPen::k_solid) {
		glDisable(GL_LINE_STIPPLE);
	}
	else {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, kPrivate::lineStipple(style));
	}
}
