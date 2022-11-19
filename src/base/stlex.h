#pragma once
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>


namespace stdx
{

	/// Returns true if the vector is sorted.
	template<typename T>
	bool is_sorted(const std::vector<T> &v)
	{
		std::is_sorted(v.begin(), v.end());
	}


	template<class T>
	bool is_ascending(const std::vector<T>& v)
	{
		for (size_t i = 1; i < v.size(); i++)
			if (v[i] < v[i - 1]) return false;

		return true;
	}


	template<class T>
	bool is_descending(const std::vector<T>& v)
	{
		for (size_t i = 1; i < v.size(); i++)
			if (v[i] > v[i - 1]) return false;

		return true;
	}


	/// Removes duplicate elements from a vector.
	template<typename T>
	void uniq(std::vector<T>& v, bool already_sort = false)
	{
		if (!already_sort)
			std::sort(v.begin(), v.end());

		auto iter = std::unique(v.begin(), v.end());
		v.resize(std::distance(v.begin(), iter));
	}


	template<class T>
	auto add_unique(std::vector<T>& v, const T& ele)
	{
		assert(is_ascending(v));

		auto iter = std::lower_bound(v.begin(), v.end(), ele);
		if (iter == v.end() || *iter != ele)
			iter = v.insert(iter, ele);

		return std::distance(v.begin(), iter);
	}


	template<class T>
	auto binary_search(const std::vector<T>& v, const T& ele)
	{
		assert(is_ascending(v));
		auto iter = std::lower_bound(v.begin(), v.end(), ele);
		return  (iter != v.end() && *iter == ele) ? std::distance(v.begin(), iter) : -1;
	}


	/// Returns true if the vector is sorted and contains each element only once.
	template<typename T>
	bool is_sort_and_uniqe(const std::vector<T> &v)
	{
		auto iter = v.begin(), end = v.end();
		if (iter == end) return true;
		while (true) {
			auto next_iter = std::next(iter);
			if (next_iter == end) return true;  // end of loop and nothing out of order
			if (*next_iter <= *iter) return false;
			iter = next_iter;
		}
	}


	/// Copies the elements of a set to a vector.
	template<class T>
	void copy(const std::set<T> &s, std::vector<T>& v)
	{
		// copies members of s into v, in sorted order from lowest to highest
		// (because the set was in sorted order).
		v.resize(s.size());
		std::copy(s.begin(), s.end(), v.begin());
	}


	template<class T>
	void copy(const std::unordered_set<T> &s, std::vector<T>& v)
	{
		v.resize(s.size());
		std::copy(s.begin(), s.end(), v.begin());
	}


	/// Copies the contents of a vector to a set.
	template<class A>
	void copy(const std::vector<A> &v, std::set<A>& s)
	{
		s.clear();
		auto iter = v.begin(), end = v.end();
		for (; iter != end; ++iter)
			s.insert(s.end(), *iter);
	}


	/// Copies the contents a vector of one type to a vector
	/// of another type.
	template<typename A, typename B>
	void copy(const std::vector<A> &from, std::vector<B>& to)
	{
		to.clear();
		to.reserve(from.size());
		for (const auto& i : from)
			to.push_back(static_cast<B>(i));
	}


	/// Copies the (key, value) pairs in a map to a vector of pairs.
	template<class A, class B>
	void copy(const std::map<A, B> &m, std::vector<std::pair<A, B> >& v)
	{
		v.resize(m.size());
		auto miter = m.begin(), mend = m.end();
		auto viter = v.begin();
		for (; miter != mend; ++miter, ++viter)
			*viter = { miter->first, miter->second };
	}


	/// Copies the keys in a map to a vector.
	template<class A, class B>
	void copy_keys(const std::map<A, B> &m, std::vector<A>& v)
	{
		v.resize(m.size());
		auto miter = m.begin(), mend = m.end();
		auto viter = v.begin();
		for (; miter != mend; ++miter, ++viter)
			*viter = miter->first;
	}


	/// Copies the values in a map to a vector.
	template<class A, class B>
	void copy_values(const std::map<A, B> &m, std::vector<B>& v)
	{
		v.resize(m.size());
		auto miter = m.begin(), mend = m.end();
		auto viter = v.begin();
		for (; miter != mend; ++miter, ++viter)
			*viter = miter->second;
	}


	/// Copies the keys in a map to a set.
	template<class A, class B>
	void copy_keys(const std::map<A, B> &m, std::set<A>& s)
	{
		s.clear();
		auto miter = m.begin(), mend = m.end();
		for (; miter != mend; ++miter)
			s.insert(s.end(), miter->first);
	}


	/// Copies the values in a map to a set.
	template<class A, class B>
	void copy_values(const std::map<A, B> &m, std::set<B>& s)
	{
		s.clear();
		auto miter = m.begin(), mend = m.end();
		for (; miter != mend; ++miter)
			s.insert(s.end(), miter->second);
	}


	/// Deletes any non-NULL pointers in the vector v, and sets
	/// the corresponding entries of v to NULL
	template<class A>
	void destroy_elements(std::vector<A*>& v)
	{
		for (auto& ptr : v) {
			if (ptr != nullptr)
				delete ptr;
			ptr = nullptr;
		}
	}


	/// Returns true if the vector of pointers contains NULL pointers.
	template<class A>
	bool contains_nullptr(const std::vector<A*> &v)
	{
		for (auto& ptr : v) {
			if (ptr == nullptr)
				return true;
		}

		return false;
	}


	/// Reverses the contents of a vector.
	template<typename T>
	void reverse(std::vector<T>& v)
	{
		std::reverse(v.begin(), v.end());
	}


	// 取两个集合的并集，即s1 + s2
	template<typename T1, typename T2 = T1, typename T3 = T2>
	T3 unite(const T1& s1, const T2& s2)
	{
		T3 r(s1.begin(), s1.end());
		for (auto& i2 : s2)
			r.insert(i2);

		return r;
	}


	// 取两个集合的交集，即s1 & s2
	template<typename T1, typename T2 = T1, typename T3 = T2>
	T3 intersect(const T1& s1, const T2& s2)
	{
		T3 r;
		for (auto& i1 : s1)
			if (s2.find(i1) != s2.end())
				r.insert(i1);

		return r;
	}


	// 取两个集合的差集：在s1但不在s2的元素集合，即s1 - s2
	template<typename T1, typename T2 = T1, typename T3 = T2>
	T3 diff(const T1& s1, const T2& s2)
	{
		T3 r(s1.begin(), s1.end());
		for (auto& i2 : s2) {
			auto iter = r.find(i2);
			if (iter != r.end())
				r.erase(iter);
		}

		return r;
	}


	// 判断s1是否包含于或等于s2，即s1 <= s2
	template<typename T1, typename T2 = T1>
	bool is_sub_of(const T1& s1, const T2& s2)
	{
		for (auto& i1 : s1)
			if (s2.find(i1) == s2.end())
				return false;

		return true;
	}

}