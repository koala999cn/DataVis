#pragma once


class KuExtrapolate
{
public:

    /// 几种对超范围坐标x的外插

    static double nearest(double low, double high, double x);

    static double mirror(double low, double high, double x);

    static double period(double low, double high, double x);

private:
	KuExtrapolate() = delete;
};
