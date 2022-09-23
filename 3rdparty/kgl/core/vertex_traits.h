#pragma once
#include <cstdint>
#include "../base/KtHolder.h"
#include "../base/traits_helper.h"


template<typename VERTEX_TYPE>
struct vertex_traits
{
	using vertex_type = VERTEX_TYPE;


	// 可具有的静态成员函数
	//static auto outdegree(VERTEX_TYPE);
	//static auto indegree(VERTEX_TYPE);

	//static auto outedges(VERTEX_TYPE);
	//static auto inedges(VERTEX_TYPE);

	//static auto weight(VERTEX_TYPE);

	//static auto edgeindex(VERTEX_TYPE); 仅对flat图有效

	template<typename T = vertex_type, 
		typename = std::enable_if_t<std::is_integral_v<T>>>
	static T edgeindex(const T& v) { 
		return v; 
	}

	template<typename T = vertex_type, 
		typename = std::enable_if_t<std::is_integral_v<T> && !std::is_const_v<T>>>
	static T& edgeindex(T& v) { 
		return v; 
	}
};


namespace kPrivate
{
	template<typename VERTEX_TYPE>
	class KtFlatVertexWrapper_ : public KtHolder<VERTEX_TYPE>
	{
	public:
		using super_ = KtHolder<VERTEX_TYPE>;

		unsigned edgeindex() const { return edgeindex_; }
		unsigned& edgeindex() { return edgeindex_; }

	private:
		unsigned edgeindex_;
	};

	template<>
	struct KtFlatVertexWrapper_<void> : public KtHolder<unsigned>
	{
	public:
		decltype(auto) edgeindex() const { return inside(); }
		decltype(auto) edgeindex() { return inside(); }
	};
};


template<typename VERTEX_TYPE>
struct vertex_traits<kPrivate::KtFlatVertexWrapper_<VERTEX_TYPE>>
	: public vertex_traits<VERTEX_TYPE>
{
	using vertex_type = kPrivate::KtFlatVertexWrapper_<VERTEX_TYPE>;

	static decltype(auto) edgeindex(const vertex_type& v) { return v.edgeindex(); }
	static decltype(auto) edgeindex(vertex_type& v) { return v.edgeindex(); }
};


template<typename VERTEX_TYPE>
struct vertex_traits<const kPrivate::KtFlatVertexWrapper_<VERTEX_TYPE>>
	: public vertex_traits<VERTEX_TYPE>
{
	using vertex_type = const kPrivate::KtFlatVertexWrapper_<VERTEX_TYPE>;

	static decltype(auto) edgeindex(vertex_type& v) { return v.edgeindex(); }
};


namespace kPrivate
{
	HAS_STATIC_MEMBER(edgeindex);
	HAS_STATIC_MEMBER(outdegree);
	HAS_STATIC_MEMBER(outedges);
	HAS_STATIC_MEMBER(inedges);
	HAS_STATIC_MEMBER(weight);
}


// 根据VERTEX_TRAITS生成有关bool常量
template<typename VERTEX_TRAITS>
struct vertex_traits_helper
{
	using vertex_type = typename VERTEX_TRAITS::vertex_type;

	constexpr static bool has_edgeindex = kPrivate::has_static_member_edgeindex<VERTEX_TRAITS, vertex_type>::value;
	constexpr static bool has_outdegree = kPrivate::has_static_member_outdegree<VERTEX_TRAITS, vertex_type>::value;
	constexpr static bool has_outedges = kPrivate::has_static_member_outedges<VERTEX_TRAITS, vertex_type>::value;
	constexpr static bool has_inedges = kPrivate::has_static_member_inedges<VERTEX_TRAITS, vertex_type>::value;
	constexpr static bool has_weight = kPrivate::has_static_member_weight<VERTEX_TRAITS, vertex_type>::value;
};


namespace kPrivate
{
	template<typename VERTEX_TYPE>
	constexpr static bool hasEdgeIndex_() {
		return vertex_traits_helper<vertex_traits<VERTEX_TYPE>>::has_edgeindex;
	}

	template<typename VERTEX_TYPE>
	constexpr static bool hasOutDegree_() {
		return vertex_traits_helper<vertex_traits<VERTEX_TYPE>>::has_outdegree;
	}

	template<typename VERTEX_TYPE>
	constexpr static bool hasWeight_() {
		return vertex_traits_helper<vertex_traits<VERTEX_TYPE>>::has_weight;
	}
}


template<typename VERTEX_TYPE>
inline constexpr bool has_edgeindex_v = kPrivate::hasEdgeIndex_<VERTEX_TYPE>();

template<typename VERTEX_TYPE>
inline constexpr bool has_outdegree_v = kPrivate::hasOutDegree_<VERTEX_TYPE>();

template<typename VERTEX_TYPE>
inline constexpr bool has_weight_v = kPrivate::hasWeight_<VERTEX_TYPE>();


// 若VERTEX_TYPE不带边偏移信息，使用KtFlatVertexWrapper_附加一个
template<typename VERTEX_TYPE>
using vertex_has_edgeindex_t = std::conditional_t<std::is_void_v<VERTEX_TYPE>, unsigned,
	std::conditional_t<has_edgeindex_v<VERTEX_TYPE>,
	VERTEX_TYPE, kPrivate::KtFlatVertexWrapper_<VERTEX_TYPE>>>;