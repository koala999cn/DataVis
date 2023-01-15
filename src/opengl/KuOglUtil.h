#pragma once


class KuOglUtil
{
public:

	// 根据style设置opengl的line-stipple状态
	// style取值参考KpPen::KeStyle
	static void glLineStyle(int style);

private:
	KuOglUtil() = delete;
};
