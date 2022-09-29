#pragma once
#include <algorithm>


template<typename ITER_TYPE>
class KtRange
{
public:
    using iterator = ITER_TYPE;
    using const_iterator = ITER_TYPE; // std::add_const_t<iterator>;
    using element_type = decltype(*std::declval<iterator>());
    using const_element_type = std::add_const_t<element_type>;
    using value_type = std::decay_t<element_type>;
    constexpr static bool is_const = std::is_const_v<element_type>;


    KtRange() = default;
    KtRange(const KtRange&) = default;
    KtRange(KtRange&&) = default;
    KtRange& operator=(const KtRange&) = default;
    KtRange& operator=(KtRange&&) = default;

    KtRange(const iterator& first, const iterator& last) :
        first_(first), last_(last) {}

    KtRange(const iterator& first, unsigned count) :
        first_(first), last_(std::next(first, count)) {}

    
    void reset(const iterator& first, const iterator& last) {
        first_ = first, last_ = last;
    }
    
    bool operator==(const KtRange& rhs) const {
        return first_ == rhs.first_ && last_ == rhs.last_;
    }

    bool operator!=(const KtRange& rhs) const {
        return first_ != rhs.first_ || last_ != rhs.last_;
    }

    // 作为容器使用

	iterator begin() { return first_; }
	iterator end() { return last_; }
    const_iterator begin() const { return first_; }
    const_iterator end() const { return last_; }
    const_iterator cbegin() const { return first_; }
    const_iterator cend() const { return last_; }


    auto size() const { return std::distance(first_, last_); }

    auto front() { return *first_; }
    auto front() const { return *first_; }
    auto back() { return *(last_ - 1); }
    auto back() const { return *(last_ - 1); }

    decltype(auto) at(unsigned idx) { return *std::next(first_, idx); }
    decltype(auto) at(unsigned idx) const { return *std::next(first_, idx); }

    KtRange subrange(unsigned offset, unsigned count) const {
        if (offset > size()) offset = size();
        if (offset + count > size()) count = size() - offset;

        return KtRange(std::next(first_, offset), count);
    }

    KtRange first(unsigned count) const {
        return subrange(0, count);
    }

    KtRange last(unsigned count) const {
        return subrange(size() - count, count);
    }


    // 作为迭代器使用

	iterator& operator++() { return ++first_; }

    void advance(unsigned n) { std::advance(first_, n); }

    decltype(auto) operator*() { return *first_; }
    decltype(auto) operator*() const { return *first_; }

    bool empty() const { return first_ == last_; }

    // advance first_ until Pred(first_)
    template<typename Pred>
    void skipUntil(Pred pred) {    
        while (!empty() && !pred(first_))
            ++first_;
    }


    // 对一些常用std算法的封装

    void sort() { std::sort(first_, last_); }
    template<class Compare> void sort(Compare comp) { std::sort(first_, last_, comp); }

    void reverse() { std::reverse(first_, last_); }

    void fill(const value_type& val) { std::fill(first_, last_, val); }

    auto count(const value_type& val) const { return std::count(first_, last_, val); }


private:
	iterator first_, last_;
};
