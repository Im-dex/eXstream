#pragma once

#include "type_traits.hpp"
#include "container_traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <iterator>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {
namespace detail {

CPPSTREAM_DEFINE_HAS_TYPE_MEMBER(difference_type)
CPPSTREAM_DEFINE_HAS_TYPE_MEMBER(value_type)
CPPSTREAM_DEFINE_HAS_TYPE_MEMBER(pointer)
CPPSTREAM_DEFINE_HAS_TYPE_MEMBER(reference)
CPPSTREAM_DEFINE_HAS_TYPE_MEMBER(iterator_category)

template <typename T>
struct is_iterator
{
    using traits = std::iterator_traits<T>;
    using result = std::conjunction<
        has_difference_type_member<traits>,
        has_value_type_member<traits>,
        has_pointer_member<traits>,
        has_reference_member<traits>,
        has_iterator_category_member<traits>
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
        cppstream::is_forward_iterator<decltype(std::begin(x))>,
        cppstream::is_forward_iterator<decltype(std::end(x))>,
        cppstream::is_forward_iterator<decltype(std::begin(y))>,
        cppstream::is_forward_iterator<decltype(std::end(y))>
    >{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T&>(), std::declval<const T&>()));
};

template <typename T>
struct is_reverse_iterable
{
    template <typename U1, typename U2>
    static auto check(U1& x, const U2& y) -> decltype(std::conjunction<
        cppstream::is_forward_iterator<decltype(std::rbegin(x))>,
        cppstream::is_forward_iterator<decltype(std::rend(x))>,
        cppstream::is_forward_iterator<decltype(std::rbegin(y))>,
        cppstream::is_forward_iterator<decltype(std::rend(y))>
    >{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T&>(), std::declval<const T&>()));
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

template <typename T, typename R = std::void_t<>>
struct is_sizable : public std::false_type {};

template <typename T>
struct is_sizable<T, std::void_t<decltype(std::declval<const T>().size())>> : public std::bool_constant<std::numeric_limits<decltype(std::declval<const T>().size())>::is_integer> {};

template <typename T>
constexpr bool is_sizable_v = is_sizable<T>::value;

template <typename T>
using is_ordered = std::bool_constant<container_traits<T>::is_ordered>;

template <typename T>
constexpr bool is_ordered_v = container_traits<T>::is_ordered;

template <typename T>
using is_distinct = std::bool_constant<container_traits<T>::is_distinct>;

template <typename T>
constexpr bool is_distinct_v = container_traits<T>::is_distinct;

template <typename T>
using value_type = std::conditional_t<
    sizeof(T) <= sizeof(intptr_t),
    T,
    T&
>;

} // cppstream namespace
