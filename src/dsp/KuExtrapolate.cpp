#include "KuExtrapolate.h"
#include <cmath>
#include <assert.h>


double KuExtrapolate::nearest(double low, double high, double x) 
{
    return x > high ? high : low;
}


double KuExtrapolate::mirror(double low, double high, double x) 
{
    auto length = high - low;
    if (x > high) {
        auto delta = std::fmod(x - high, length);
        return high - delta;
    }
    else {
        auto delta = std::fmod(low - x, length);
        return low + delta;
    }
}


double KuExtrapolate::period(double low, double high, double x) 
{
    auto length = high - low;
    if (x >= high) {
        auto delta = std::fmod(x - low, length);
        return low + delta;
    }
    else {
        auto delta = std::fmod(low - x, length);
        return high - delta;
    }

    assert(x >= low && x < high);
}
