#pragma once

#include "config.hpp"
#include "utility.hpp"

STREAM_SUPRESS_ALL_WARNINGS
#include <type_traits>
#include <limits>
STREAM_RESTORE_ALL_WARNINGS

#define STREAM_DEFINE_HAS_TYPE(typeName)\
    template <typename T>\
    struct stream_detail_has_ ## typeName ## _type\
    {\
        template <typename U>\
        static std::true_type check(typename U::typeName*);\
        \
        template <typename>\
        static std::false_type check(...);\
        \
        using result = decltype(check<T>(nullptr));\
    };\
    \
    template <typename T>\
    using has_ ## typeName ## _type = typename stream_detail_has_ ## typeName ## _type<T>::result;\
    \
    template <typename T>\
    constexpr bool has_ ## typeName ## _type_v = has_ ## typeName ## _type<T>::value;

#define STREAM_OP_PREFIX(op) op value
#define STREAM_OP_POSTFIX(op) value op

#define STREAM_DEFINE_HAS_UNARY_OPERATOR_I(name, expression)\
    namespace detail {\
        template <typename T>\
        struct name final\
        {\
            template <typename U>\
            static auto check(U&& value) -> decltype(expression, std::true_type{});\
            \
            static std::false_type check(...);\
            \
            using result = decltype(check(std::declval<T>()));\
        };\
    } /* detail namespace*/\
    \
    template <typename T>\
    using name = typename detail::name<T>::result;\
    \
    template <typename T>\
    constexpr bool name ## _v = name<T>::value;

#define STREAM_DEFINE_HAS_UNARY_OPERATOR(op, name, opPosition)\
    STREAM_DEFINE_HAS_UNARY_OPERATOR_I(name, opPosition(op))

namespace std {

#if defined(STREAM_GCC) || defined(STREAM_CLANG)
    // TODO: define bool_constant, is_*_v etc
#endif

namespace detail {

/*template <typename T, typename U>
struct is_swapable_with_helper
{
    template <typename A, typename B>
    static auto check(const A&, const B&) -> decltype(swap(declval<A>(), declval<B>()), true_type{});

    static false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

template <typename T, typename U>
struct is_nothrow_swapable_with_helper
{
    template <typename A, typename B>
    static auto check(const A&, const B&) -> decltype(noexcept(swap(declval<A>(), declval<B>())), true_type{});

    static false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

template <typename T, typename U>
using is_swapable_with_helper_t = typename is_swapable_with_helper<T, U>::result;

template <typename T, typename U>
using is_nothrow_swapable_with_helper_t = typename is_nothrow_swapable_with_helper<T, U>::result;*/

} // detail namespace

#ifdef STREAM_MSVC

template <typename T, typename U>
using is_swappable_with = _Is_swappable_with<T, U>;

template <typename T, typename U>
using is_nothrow_swappable_with = _Is_nothrow_swappable_with<T, U>;

#endif

template <typename T, typename U>
constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template <typename T>
using is_swappable = is_swappable_with<T, T>;

template <typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;

template <typename T, typename U>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

template <typename T>
using is_nothrow_swappable = is_nothrow_swappable_with<T, T>;

template <typename T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

} // std namespace

namespace stream {

template <typename T>
using remove_cvr = std::remove_cv<std::remove_reference_t<T>>;

template <typename T>
using remove_cvr_t = typename remove_cvr<T>::type;

namespace detail {

template <typename T, typename U>
struct is_comparable_to final
{
    template <typename A, typename B>
    static auto check(A&& a, B&& b) -> decltype(a == b, std::true_type{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

} // detail namespace

template <typename T, typename U>
using is_comparable_to = typename detail::is_comparable_to<T, U>::result;

template <typename T, typename U>
constexpr bool is_comparable_to_v = is_comparable_to<T, U>::value;

template <typename T>
using is_comparable = is_comparable_to<T, T>;

template <typename T>
constexpr bool is_comparable_v = is_comparable<T>::value;

namespace detail {

template <typename T, typename U>
struct is_nothrow_comparable_to final
{
    template <typename A, typename B>
    static auto check(A&& a, B&& b) -> decltype(std::bool_constant<noexcept(a == b)>{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<U>()));
};

} // detail namespace

template <typename T, typename U>
using is_nothrow_comparable_to = typename detail::is_nothrow_comparable_to<T, U>::result;

template <typename T, typename U>
constexpr bool is_nothrow_comparable_to_v = is_nothrow_comparable_to<T, U>::value;

template <typename T>
using is_nothrow_comparable = is_nothrow_comparable_to<T, T>;

template <typename T>
constexpr bool is_nothrow_comparable_v = is_nothrow_comparable<T>::value;

STREAM_DEFINE_HAS_UNARY_OPERATOR(*, has_unary_operator_asterisk, STREAM_OP_PREFIX)
STREAM_DEFINE_HAS_UNARY_OPERATOR(++, has_prefix_increment_operator, STREAM_OP_PREFIX)
STREAM_DEFINE_HAS_UNARY_OPERATOR(++, has_postfix_increment_operator, STREAM_OP_POSTFIX)

namespace detail {

template <typename T>
struct has_unary_operator_asterisk<T*> final
{
    using result = std::true_type;
};

} // detail namespace

namespace detail {

template <bool Valid, typename IntType, IntType... Ints>
struct int_sum
{
};

template <typename IntType, IntType Head, IntType... Tail>
struct int_sum<true, IntType, Head, Tail...> : public std::integral_constant<IntType, Head + int_sum<true, IntType, Tail...>::value>
{
};

template <typename IntType, IntType Head>
struct int_sum<true, IntType, Head> : public std::integral_constant<IntType, Head>
{
};

template <typename IntType>
struct int_sum<true, IntType> : public std::integral_constant<IntType, std::numeric_limits<IntType>::epsilon()>
{
};

} // detail namespace

template <typename IntType, IntType... Ints>
struct int_sum : public detail::int_sum<std::numeric_limits<IntType>::is_integer, IntType, Ints...>
{
    static_assert(std::numeric_limits<IntType>::is_integer, "IntType should be integer type");
};

template <typename IntType, IntType... Ints>
constexpr auto int_sum_v = int_sum<IntType, Ints...>::value;

namespace detail {

template <typename F, typename R, typename... Args>
struct is_callable
{
    template <typename T, typename... Ts>
    static auto check(T&& fn, Ts&&... args) -> decltype(std::is_same<decltype(fn(std::forward<Ts>(args)...)), R>{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<F>(), std::declval<Args>()...));
};

} // detail namespace

template <typename F, typename R, typename... Args>
using is_callable = typename detail::is_callable<F, R, Args...>::result;

template <typename F, typename R, typename... Args>
constexpr bool is_callable_v = is_callable<F, R, Args...>::value;

namespace detail {

template <typename F, typename... Args>
struct is_invokable
{
    template <typename T, typename... Ts>
    static auto check(T&& fn, Ts&&... args) -> decltype(fn(std::forward<Ts>(args)...), std::true_type{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<F>(), std::declval<Args>()...));
};

} // detail namespace

template <typename F, typename... Args>
using is_invokable = typename detail::is_invokable<F, Args...>::result;

template <typename F, typename... Args>
constexpr bool is_invokable_v = is_invokable<F, Args...>::value;

namespace detail {

template <bool Valid, typename F, typename... Args>
struct result_of
{
    using type = error_t;
};

template <typename F, typename... Args>
struct result_of<true, F, Args...>
{
    using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

} // detail namespace

template <typename F, typename... Args>
using result_of = typename detail::result_of<is_invokable_v<F, Args...>, F, Args...>::type;

} // stream namespace

#undef STREAM_OP_PREFIX
#undef STREAM_OP_POSTFIX
#undef STREAM_DEFINE_HAS_UNARY_OPERATOR_I
#undef STREAM_DEFINE_HAS_UNARY_OPERATOR
