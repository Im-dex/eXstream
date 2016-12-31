#pragma once

#include "type_traits.hpp"
#include "ordering_traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <iterator>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {
namespace detail {

CPPSTREAM_DEFINE_HAS_TYPE(difference_type)
CPPSTREAM_DEFINE_HAS_TYPE(value_type)
CPPSTREAM_DEFINE_HAS_TYPE(pointer)
CPPSTREAM_DEFINE_HAS_TYPE(reference)
CPPSTREAM_DEFINE_HAS_TYPE(iterator_category)

template <typename T>
struct is_iterator
{
    using traits = std::iterator_traits<T>;
    using result = std::conjunction<
        has_difference_type<traits>,
        has_value_type<traits>,
        has_pointer<traits>,
        has_reference<traits>,
        has_iterator_category<traits>
    >;
};

template <typename T>
struct is_iterator<T*>
{
    using result = std::true_type;
};

} // detail namespaces

template <typename T>
using is_iterator = typename detail::is_iterator<T>::result;

template <typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

namespace detail {

template <bool IsIterator, typename T>
struct is_forward_iterator : public std::false_type
{
};

template <typename T>
struct is_forward_iterator<true, T> : public std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<T>::iterator_category>
{
};

} // detail namespace

template <typename T>
using is_forward_iterator = detail::is_forward_iterator<is_iterator_v<T>, T>;

template <typename T>
constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;

namespace detail {

template <bool IsIterator, typename T>
struct is_const_iterator : public std::false_type
{
};

template <typename T>
struct is_const_iterator<true, T> : public std::is_const<std::remove_reference_t<typename std::iterator_traits<T>::reference>>
{
};

} // detail namespace

template <typename T>
using is_const_iterator = detail::is_const_iterator<is_iterator_v<T>, T>;

template <typename T>
constexpr bool is_const_iterator_v = is_const_iterator<T>::value;

namespace detail {

template <typename T>
struct is_iterable
{
    template <typename U1, typename U2>
    static auto check(U1& x, const U2& y) -> decltype(std::conjunction<
        cppstream::is_forward_iterator<decltype(x.begin())>,
        cppstream::is_forward_iterator<decltype(x.end())>,
        cppstream::is_forward_iterator<decltype(y.begin())>,
        cppstream::is_forward_iterator<decltype(y.end())>
    >{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T&>(), std::declval<const T&>()));
};

template <typename T>
struct is_reverse_iterable
{
    template <typename U>
    static auto check(U& x, const U& y) -> decltype(std::conjunction<
        cppstream::is_forward_iterator<decltype(x.rbegin())>,
        cppstream::is_forward_iterator<decltype(x.rend())>,
        cppstream::is_forward_iterator<decltype(y.rbegin())>,
        cppstream::is_forward_iterator<decltype(y.rend())>
    >{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T>(), std::declval<T>()));
};

} // detail namespace

template <typename T>
using is_iterable = typename detail::is_iterable<T>::result;

template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
using is_reverse_iterable = typename detail::is_reverse_iterable<T>::result;

template <typename T>
constexpr bool is_reverse_iterable_v = is_reverse_iterable<T>::value;

namespace detail {

template <bool Valid, typename T>
struct is_sizable_check_result
{
    using type = std::false_type;
};

template <typename T>
struct is_sizable_check_result<true, T>
{
    using type = std::bool_constant<std::numeric_limits<decltype(std::declval<T>().size())>::is_integer>;
};

template <typename T>
struct is_sizable
{
    template <typename U>
    static auto check(U&& value) -> decltype(value.size(), std::true_type{});

    static std::false_type check(...);

    using has_method = decltype(check(std::declval<T>()));

    using result = typename is_sizable_check_result<has_method::value, T>::type;
};

} // detail namespace

template <typename T>
using is_sizable = typename detail::is_sizable<T>::result;

template <typename T>
constexpr bool is_sizable_v = is_sizable<T>::value;

template <typename T>
using is_ordered = std::bool_constant<ordering_traits<T>::is_ordered>;

template <typename T>
constexpr bool is_ordered_v = ordering_traits<T>::is_ordered;

template <typename T>
using value_type = std::conditional_t<
    sizeof(T) <= sizeof(intptr_t),
    T,
    T&
>;

} // cppstream namespace
