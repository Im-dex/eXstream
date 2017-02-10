#pragma once

#include "type_traits.hpp"
#include "container_traits.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <iterator>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {
namespace detail {

EXSTREAM_DEFINE_HAS_TYPE_MEMBER(difference_type)
EXSTREAM_DEFINE_HAS_TYPE_MEMBER(value_type)
EXSTREAM_DEFINE_HAS_TYPE_MEMBER(pointer)
EXSTREAM_DEFINE_HAS_TYPE_MEMBER(reference)
EXSTREAM_DEFINE_HAS_TYPE_MEMBER(iterator_category)

EXSTREAM_DEFINE_HAS_METHOD(reserve)
EXSTREAM_DEFINE_HAS_METHOD(append)
EXSTREAM_DEFINE_HAS_METHOD(build)
EXSTREAM_DEFINE_HAS_METHOD(builder)

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

template <typename T, typename Tag, bool IsIterator = false>
struct is_iterator_with_tag_impl : std::false_type {};

template <typename T, typename Tag>
struct is_iterator_with_tag_impl<T, Tag, true> : std::is_base_of<Tag, typename std::iterator_traits<T>::iterator_category> {};

template <typename T, typename Tag>
using is_iterator_with_tag = is_iterator_with_tag_impl<T, Tag, is_iterator_v<T>>;

} // detail namespace

template <typename T>
using is_output_iterator = detail::is_iterator_with_tag<T, std::output_iterator_tag>;

template <typename T>
constexpr bool is_output_iterator_v = is_output_iterator<T>::value;

template <typename T>
using is_input_iterator = detail::is_iterator_with_tag<T, std::input_iterator_tag>;

template <typename T>
constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

template <typename T>
using is_forward_iterator = detail::is_iterator_with_tag<T, std::forward_iterator_tag>;

template <typename T>
constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;

template <typename T>
using is_random_access_iterator = detail::is_iterator_with_tag<T, std::random_access_iterator_tag>;

template <typename T>
constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;

namespace detail {

template <bool IsIterator, typename T>
struct is_const_iterator : std::false_type
{
};

template <typename T>
struct is_const_iterator<true, T> : std::is_const<std::remove_reference_t<typename std::iterator_traits<T>::reference>>
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
        is_input_iterator<decltype(std::begin(x))>,
        is_input_iterator<decltype(std::end(x))>,
        is_input_iterator<decltype(std::begin(y))>,
        is_input_iterator<decltype(std::end(y))>
    >{});

    static std::false_type check(...);

    using result = decltype(check(std::declval<T&>(), std::declval<const T&>()));
};

template <typename T>
struct is_reverse_iterable
{
    template <typename U1, typename U2>
    static auto check(U1& x, const U2& y) -> decltype(std::conjunction<
        is_input_iterator<decltype(std::rbegin(x))>,
        is_input_iterator<decltype(std::rend(x))>,
        is_input_iterator<decltype(std::rbegin(y))>,
        is_input_iterator<decltype(std::rend(y))>
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
struct is_sizable : std::false_type {};

template <typename T>
struct is_sizable<T, std::void_t<decltype(std::declval<const T>().size())>> : std::bool_constant<std::numeric_limits<decltype(std::declval<const T>().size())>::is_integer> {};

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

// TODO: test
template <typename T, typename Element>
using is_builder = std::conjunction<
    detail::has_reserve_method<T, size_t>,
    detail::has_append_method<T, const Element&>,
    detail::has_append_method<T, Element&&>,
    detail::has_build_method<T>
>;

template <typename T, typename Element>
constexpr bool is_builder_v = is_builder<T, Element>::value;

// TODO: test
template <typename T, typename Element, typename AlwaysVoid = std::void_t<>>
struct is_collector : std::false_type {};

template <typename T, typename Element>
struct is_collector<T, Element, std::void_t<decltype(std::declval<T>().builder(std::declval<type_t<Element>>()))>>
    : is_builder<decltype(std::declval<T>().builder(std::declval<type_t<Element>>())), Element>
{
};

template <typename T, typename Element>
constexpr bool is_collector_v = is_collector<T, Element>::value;

// TODO: test
template <typename T>
using is_any_pair = std::disjunction<is_pair<T>, is_tuple_n<2, T>>;

template <typename T>
constexpr bool is_any_pair_v = is_any_pair<T>::value;

// TODO: test
template <typename Pair, typename First, typename Second>
using is_exact_pair = std::disjunction<
    std::is_same<std::pair<First, Second>, Pair>,
    std::is_same<std::tuple<First, Second>, Pair>
>;

template <typename Pair, typename First, typename Second>
constexpr bool is_exact_pair_v = is_exact_pair<Pair, First, Second>::value;

} // exstream namespace
