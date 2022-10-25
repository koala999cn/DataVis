#include "KuExtrapolate.h"
#include <cmath>


double KuExtrapolate::nearest(double low, double high, double x) 
{
    return x > high ? high : low;
}


double KuExtrapolate::mirror(double low, double high, double x) 
{
    auto length = high - low;
    if (x > high) {
        auto ratio = std::fmod(x - high, length);
        return high - ratio * length;
    }
    else {
        auto ratio = std::fmod(low - x, length);
        return low + ratio * length;
    }
}


double KuExtrapolate::period(double low, double high, double x) 
{
    auto length = high - low;
    if (x > high) {
        auto ratio = std::fmod(x - high, length);
        return low + ratio * length;
    }
    else {
        auto ratio = std::fmod(low - x, length);
        return high - ratio * length;
    }
}
