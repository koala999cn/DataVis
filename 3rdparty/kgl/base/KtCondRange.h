#pragma once
#include <type_traits>


/// ´øÎ½´ÊµÄKtRangeÍØÕ¹

template<typename ITER_TYPE, typename PRED>
class KtCondRange
{
public:

    using iterator = ITER_TYPE;
    using const_iterator = std::add_const_t<iterator>;
    using deref_type = decltype(*std::declval<iterator>());
    using const_deref_type = decltype(*std::declval<const_iterator>());
    using value_type = std::remove_reference_t<deref_type>;
    constexpr static bool is_const = std::is_const<deref_type>::value;

    KtCondRange() = default;

    KtCondRange(const_iterator& first, const_iterator& last, PRED pred)
        : first_(first), last_(last), pred_(pred) {
        meetCond_();
    }

    KtCondRange(iterator&& first, iterator&& last, PRED pred)
        : first_(std::move(first)), last_(std::move(last))
        , pred_(pred) {
        meetCond_();
    }


    KtCondRange(const_iterator& first, unsigned count, PRED pred)
        : first_(first), last_(std::next(first, count))
        , pred_(pred) {
        meetCond_();
    }

    KtCondRange(iterator&& first, unsigned count, PRED pred)
        : first_(std::move(first)), last_(std::next(first_, count))
        , pred_(pred) {
        meetCond_();
    }


    KtCondRange(const KtCondRange& rhs)
        : first_(rhs.first_), last_(rhs.last_), pred_(rhs.pred_) {
        meetCond_();
    }

    KtCondRange& operator=(const KtCondRange& rhs) {
        first_ = rhs.first_, last_ = rhs.last_;
        pred_ = rhs, pred_;
        return *this;
    }

    void reset(const iterator& first, const iterator& last) {
        first_ = first, last_ = last;
    }


    bool operator==(const KtCondRange& rhs) const {
        return first_ == rhs.first_ && last_ == rhs.last_;
    }

    const_iterator begin() const { return first_; }
    iterator begin() { return first_; }
    const_iterator end() const { return last_; }
    iterator end() { return last_; }

    iterator& operator++() { 
        ++first_; meetCond_();
        return first_;
    }

    decltype(auto) operator*() { return *first_; }
    decltype(auto) operator*() const { return *first_; }

    bool empty() const { return first_ == last_; }


    unsigned size() const {
        unsigned s(0);
        for (auto iter = first_; iter != last_; s++) {
            do {
                ++iter;
            } while (iter != last_ && !pred_(*iter));
        }

        return s;
    }


    void meetCond_() {
        while (!empty() && !pred_(*first_))
            ++first_;
    }


private:
    iterator first_, last_;
    PRED pred_;
};