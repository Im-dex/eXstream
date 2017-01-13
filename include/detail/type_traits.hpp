#pragma once

#include "constexpr_if.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <limits>
EXSTREAM_RESTORE_ALL_WARNINGS

#define EXSTREAM_DEFINE_HAS_TYPE_MEMBER(typeName)\
    template <typename T, typename U = std::void_t<>>\
    struct has_ ## typeName ## _member : public std::false_type {};\
    \
    template <typename T>\
    struct has_ ## typeName ## _member<T, std::void_t<typename T::typeName>> : public std::true_type {};\
    \
    template <typename T>\
    constexpr bool has_ ## typeName ## _member_v = has_ ## typeName ## _member<T>::value;

namespace std {

#if defined(EXSTREAM_GCC) || defined(EXSTREAM_CLANG)
    // TODO: define bool_constant, is_*_v etc
#endif

#ifdef EXSTREAM_MSVC

template <typename T, typename U>
using is_swappable_with = _Is_swappable_with<T, U>;

template <typename T, typename U>
using is_nothrow_swappable_with = _Is_nothrow_swappable_with<T, U>;

#else
    // TODO: implement
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

namespace exstream {

template <typename T>
using remove_cvr = std::remove_cv<std::remove_reference_t<T>>;

template <typename T>
using remove_cvr_t = typename remove_cvr<T>::type;

template <typename T, typename U, typename AlwaysVoid = std::void_t<>>
struct is_comparable_to : std::false_type {};

template <typename T, typename U>
struct is_comparable_to<T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>())>> : std::true_type {};

template <typename T, typename U>
constexpr bool is_comparable_to_v = is_comparable_to<T, U>::value;

template <typename T>
using is_comparable = is_comparable_to<T, T>;

template <typename T>
constexpr bool is_comparable_v = is_comparable<T>::value;

template <typename T, typename U, bool AlwaysVoid = is_comparable_to_v<T, U>>
struct is_nothrow_comparable_to : std::false_type {};

template <typename T, typename U>
struct is_nothrow_comparable_to<T, U, true> : std::bool_constant<noexcept(std::declval<T>() == std::declval<U>())> {};

template <typename T, typename U>
constexpr bool is_nothrow_comparable_to_v = is_nothrow_comparable_to<T, U>::value;

template <typename T>
using is_nothrow_comparable = is_nothrow_comparable_to<T, T>;

template <typename T>
constexpr bool is_nothrow_comparable_v = is_nothrow_comparable<T>::value;

template <typename T>
constexpr bool is_integer_v = std::numeric_limits<T>::is_integer;

template <typename IntType, typename... Ints>
constexpr auto sum(IntType value, Ints... tail) noexcept
{
    static_assert(is_integer_v<IntType>, "IntType should be integer type");

#ifndef EXSTREAM_MSVC
    // NOTE: MSVC 2015 error: expected constant expression
    static_assert(std::conjunction_v<is_integer<Ints>...>, "All of Ints types should be integer");
#endif

    EXSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4296)

    return constexpr_if<(sizeof...(Ints) > 0)>()
        .then([&](auto) noexcept
        {
            return value + sum(tail...);
        })
        .else_([&](auto) noexcept
        {
            return value;
        })(nothing);

    EXSTREAM_MSVC_WARNINGS_POP
}

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

template <typename T>
struct is_reference_wrapper : std::false_type {};

template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

template <typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

} // exstream namespace
