#pragma once
#include <map>


// ����std::mapʵ�ֵ�����ӳ����
// ����ɢ�㣨�ؼ��㣩������map�У����ؼ���֮��ʹ�����Բ�ֵ

template<typename KEY, typename VALUE>
class KtGradient
{
public:

	using key_type = KEY;
	using value_type = VALUE;
	
	// ���ÿ��Ƶ�
	void insert(const KEY& from, const VALUE& to) {
		stops_[from] = to;
	}

	// �Ƴ����Ƶ�from
	void erase(const KEY& from) {
		stops_.erase(from);
	}

	// �Ƿ���ڿ��Ƶ�from
	bool has(const KEY& from) {
		return stops_.find(from) != stops_.cend();
	}

	// �ƶ����Ƶ�
	void move(const KEY& oldFrom, const KEY& newFrom) {
		assert(has(oldFrom));
		auto val = stops_[oldFrom];
		erase(oldFrom);
		insert(newFrom, val);
	}

	// ����ӳ��
	VALUE map(const KEY& from) const {
		assert(!stops_.empty());

		auto upper = stops_.upper_bound(from);
		if (upper == stops_.cend())
			return std::prev(stops_.cend(), 1)->second;
		else if (upper == stops_.cbegin())
			return upper->second;

		auto lower = std::prev(upper);

		auto& x0 = lower->first;
		auto& x1 = upper->first;
		auto& y0 = lower->second;
		auto& y1 = upper->second;

		assert(x0 != x1);
		return y0 + (y1 - y0) * ((from - x0) / (x1 - x0));
	}

	unsigned size() const {
		return stops_.size();
	}

	auto& at(unsigned idx) const {
		return *std::next(stops_.cbegin(), idx);
	}

	void clear() {
		stops_.clear();
	}

	auto begin() const { return stops_.begin(); }
	auto end() const { return stops_.end(); }

	auto cbegin() const { return stops_.cbegin(); }
	auto cend() const { return stops_.cend(); }

private:
	std::map<KEY, VALUE> stops_;
};

