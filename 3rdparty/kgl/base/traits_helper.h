#pragma once


// 用于判定某个类是否具有公开的xxx成员函数
#define HAS_MEMBER(xxx) \
    template<typename T, typename... Args> \
    struct has_member_##xxx \
    { \
    private: \
        template<typename U> static auto Check(int) -> decltype(std::declval<U>().xxx(std::declval<Args>()...), std::true_type()); \
        template<typename U> static std::false_type Check(...); \
    public: \
        static constexpr auto value = decltype(Check<T>(0))::value; \
    };


// 用于判定某个类是否具有公开的xxx静态成员函数
#define HAS_STATIC_MEMBER(xxx) \
    template<typename T, typename... Args> \
    struct has_static_member_##xxx \
    { \
    private: \
        template<typename U> static auto Check(int) -> decltype(U::xxx(std::declval<Args>()...), std::true_type()); \
        template<typename U> static std::false_type Check(...); \
    public: \
        static constexpr auto value = decltype(Check<T>(0))::value; \
    };


// 用于判定两个类型是否具有重载的op操作符
#define HAS_OVERLOADED_BIOP(op, name) \
template<typename T1, typename T2> \
struct has_operator_##name \
{ \
private: \
    template<typename U1, typename U2> static auto Check(int) -> \
        decltype(std::declval<U1>() op std::declval<U2>(), std::true_type()); \
    template<typename U1, typename U2> static std::false_type Check(...); \
public: \
    static constexpr auto value = decltype(Check<T1, T2>(0))::value; \
};

HAS_OVERLOADED_BIOP(<< , output)
HAS_OVERLOADED_BIOP(>> , input)