#pragma once
#include <memory>
#include <tuple>


// ������T�ķ�װ��
// ʵ�ֻ����Ĺ��졢��ֵ��ȡֵ���Ƚϡ�ȡַ��ת�͵ȷ���

namespace kPrivate
{
	template<typename T, bool trivial>
	class KtHolderImpl_
	{
	public:
		using value_type = T;
		using const_value_type = std::add_const_t<value_type>;

		KtHolderImpl_() : val_{} {}
		KtHolderImpl_(const value_type& val) : val_(val) {}
		KtHolderImpl_(value_type&& val) : val_(std::move(val)) {}
		KtHolderImpl_(const KtHolderImpl_& h) : val_(h.val_) {}
		KtHolderImpl_(KtHolderImpl_&& h) : val_(std::move(h.val_)) {}

		template<typename... X>
		explicit KtHolderImpl_(X&&... x) : val_(std::forward<X>(x)...) {}


		KtHolderImpl_& operator=(const value_type& val) {
			val_ = val; return *this;
		}

		KtHolderImpl_& operator=(value_type&& val) {
			val_ = std::move(val); return *this;
		}

		KtHolderImpl_& operator=(const KtHolderImpl_& h) {
			val_ = h.val_; return *this;
		}

		KtHolderImpl_& operator=(KtHolderImpl_&& h) {
			val_ = std::move(h.val_); return *this;
		}


		bool operator==(const KtHolderImpl_& h) const { return val_ == h.val_; }
		bool operator!=(const KtHolderImpl_& h) const { return val_ != h.val_; }


		operator value_type& () { return val_; }
		operator const_value_type& () const { return val_; }


		value_type* operator&() { return &val_; }
		const_value_type* operator&() const { return &val_; }

		auto operator->() {
			if constexpr (std::is_pointer_v<value_type>)
				return val_;
			else
				return &val_;
		}

		auto operator->() const {
			if constexpr (std::is_pointer_v<value_type>)
				return val_;
			else
				return &val_;
		}

		value_type& inside() { return val_; }
		const_value_type& inside() const { return val_; }


	private:
		value_type val_;
	};


	template<typename T>
	class KtHolderImpl_<T, true>
	{
	public:
		using value_type = T;
		using const_value_type = std::add_const_t<value_type>;

		KtHolderImpl_() = default;
		KtHolderImpl_(const KtHolderImpl_&) = default;
		KtHolderImpl_(KtHolderImpl_&&) = default;
		~KtHolderImpl_() = default;
		KtHolderImpl_& operator=(const KtHolderImpl_&) = default;
		KtHolderImpl_& operator=(KtHolderImpl_&&) = default;

		template<typename... X>
		explicit KtHolderImpl_(X&&... x) : val_(std::forward<X>(x)...) {}
		KtHolderImpl_(const value_type& val) : val_(val) {}
		KtHolderImpl_(value_type&& val) : val_(std::move(val)) {}
		KtHolderImpl_& operator=(const value_type& val) { val_ = val; return *this; }
		KtHolderImpl_& operator=(value_type&& val) { val_ = std::move(val); return *this; }


		bool operator==(const KtHolderImpl_& h) const { return val_ == h.val_; }
		bool operator!=(const KtHolderImpl_& h) const { return val_ != h.val_; }


		operator value_type& () { return val_; }
		operator const_value_type& () const { return val_; }


		value_type* operator&() { return &val_; }
		const_value_type* operator&() const { return &val_; }

		auto operator->() {
			if constexpr (std::is_pointer_v<value_type>)
				return val_;
			else
				return &val_;
		}

		auto operator->() const {
			if constexpr (std::is_pointer_v<value_type>)
				return val_;
			else
				return &val_;
		}

		value_type& inside() { return val_; }
		const_value_type& inside() const { return val_; }


	private:
		value_type val_;
	};

}


// ��������Ԫ��
template<typename T>
using KtHolder = kPrivate::KtHolderImpl_<T, std::is_trivial_v<T>>; // TODO: ʹ��trivial copyable


// ֧�ֶ������Ԫ��
template<typename... T>
class KtTupleHolder : public KtHolder<std::tuple<T...>>
{
public:
	using super_ = KtHolder<std::tuple<T...>>;

	// ��չ���캯��
	using super_::super_;
	KtTupleHolder(T&&... t) : super_(std::make_tuple(std::forward<T>(t)...)) {}


	constexpr static auto tupleSize() {
		return std::tuple_size_v<std::tuple<T...>>;
	}


	// ��չinside��Ա����
	using super_::inside;

	template<int I>
	auto& inside() { return std::get<I>(super_::inside()); }

	template<int I>
	auto& inside() const { return std::get<I>(super_::inside()); }
};
