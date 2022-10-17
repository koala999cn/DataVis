#pragma once
#include <set>
#include <map>


template<typename FROM_T, typename TO_T>
class KtGradient
{
public:
	KtGradient() = default;
	
	// ÉèÖÃ¿ØÖÆµã
	void setAt(const FROM_T& from, const TO_T& to) {
		stops_.insert(std::pair(from, to));
	}

	TO_T getAt(const FROM_T& from) const {
		assert(!stops_.empty());
		auto lower = stops_.lower_bound(std::pair(from, TO_T()));
		if (lower == stops_.cend())
			return stops_.cbegin()->second;

		auto upper = stops_.upper_bound(std::pair(from, TO_T()));
		if (upper == stops_.cend())
			return std::prev(stops_.cend(), 1)->second;

		auto& x0 = lower->first;
		auto& x1 = upper->first;
		auto& y0 = lower->second;
		auto& y1 = upper->second;

		return y0 + (y1 - y0) * ((from - x0) / (x1 - x0));
	}

	unsigned numStops() const {
		return stops_.size();
	}

	auto& stopAt(unsigned idx) const {
		return *std::next(stops_.cbegin(), idx);
	}

private:

	using stop_type_ = std::pair<FROM_T, TO_T>;

	struct less_ {
		constexpr bool operator()(const stop_type_& lhs, const stop_type_& rhs) const
		{
			return lhs.first < rhs.first; 
		}
	};

	std::set<stop_type_, less_> stops_;
};

