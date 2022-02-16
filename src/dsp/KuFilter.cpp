#include "KuFilter.h"
#include "KtuMath.h"


kReal KuFilter::smoothstep(kReal a, kReal b, kReal x)
{
    auto c = KtuMath<kReal>::clamp((x - a) / (b - a), 0, 1);
    return c * c * (3 - 2 * c);
}
