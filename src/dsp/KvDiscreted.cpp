#include "KvDiscreted.h"

kRange KvDiscreted::valueRange(kIndex channel) const 
{
	if (size() == 0) return { 0, 0 };

	kReal vmin = std::numeric_limits<kReal>::max();
	kReal vmax = std::numeric_limits<kReal>::lowest();

	std::vector<kIndex> idx(dim(), 0);
	for (kIndex i = 0; i < size(); i++) {
		auto val = value(idx.data(), channel);
		if (std::isnan<kReal>(val))
			continue;

		vmin = std::min(vmin, val);
		vmax = std::max(vmax, val);

		nextIndex(idx.data());
	}

	return { vmin, vmax };
}