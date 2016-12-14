#pragma once

#include "type_traits.hpp"
#include "ordering_traits.hpp"

namespace stream {
namespace detail {

template <typename T>
struct has_iterator_traits_specialization
{
    template <typename U>
    static std::true_type check(typename std::iterator_traits<U>::value_type*);

    template <typename>
    static std::false_type check(...);

    using result = decltype(check<T>(nullptr));
};

template <typename T>
using has_iterator_traits_specialization_t = typename has_iterator_traits_specialization<T>::result;

template <typename T>
constexpr bool has_iterator_traits_specialization_v = has_iterator_traits_specialization_t<T>::value;

} // detail namespace

//"Iterator type should be comparable"
//"Iterator type should have unary operator '*'"
//"Iterator type should have prefix operator '++'"
//"Iterator type should have postfix operator '++'"
//"std::iterator_traits<Iterator>::value_type should exists"

template <typename T>
using is_range_iterator = std::conjunction<
                              is_comparable<T>,
                              has_unary_operator_asterisk<T>,
                              has_prefix_increment_operator<T>,
                              has_postfix_increment_operator<T>,
                              detail::has_iterator_traits_specialization_t<T>
                          >;

template <typename T>
constexpr bool is_range_iterator_v = is_range_iterator<T>::value;

namespace detail {

template <bool Valid, typename T>
struct is_range_check_result
{
    using type = std::false_type;
};

template <typename T>
struct is_range_check_result<true, T>
{
    using type = std::conjunction<
        is_range_iterator<decltype(std::declval<T>().begin())>,
        is_range_iterator<decltype(std::declval<T>().end())>
    >;
};

template <typename T>
struct is_range
{
    template <typename U>
    static auto check(U&& value) -> decltype(value.begin(), value.end(), std::true_type{});

    static std::false_type check(...);

    using has_methods = decltype(check(std::declval<T>()));

    using result = typename is_range_check_result<has_methods::value, T>::type;
};

} // detail namespace

template <typename T>
using is_range = typename detail::is_range<T>::result;

template <typename T>
constexpr bool is_range_v = is_range<T>::value;

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
constexpr bool is_ordered_v = is_ordered<T>::value;

template <typename T, bool Const>
class iterator;

namespace detail {

template <typename T>
struct is_iterator final
{
    template <typename U, bool Const>
    static std::true_type check(iterator<U, Const>*);

    static std::false_type check(...);

    using check_result = decltype(check(std::declval<T*>()));
    using result = std::conjunction<check_result, is_comparable<T>>;
};

template <typename T>
struct is_const_iterator final
{
    template <typename U>
    static std::true_type check(iterator<U, true>*);

    static std::false_type check(...);

    using check_result = decltype(check(std::declval<T*>()));
    using result = std::conjunction<check_result, is_comparable<T>>;
};

} // detail namespace

template <typename T>
using is_iterator = typename detail::is_iterator<T>::result;

template <typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

template <typename T>
using is_const_iterator = typename detail::is_const_iterator<T>::result;

template <typename T>
constexpr bool is_const_iterator_v = is_const_iterator<T>::value;

template <typename T, typename Iterator, typename ConstIterator>
class iterable;

template <typename T, typename Iterator, typename ConstIterator>
class reverse_iterable;

namespace detail {

template <typename T>
struct is_iterable final
{
    template <typename U, typename Iterator, typename ConstIterator>
    static std::true_type check(iterable<U, Iterator, ConstIterator>*);

    static std::false_type check(...);

    using result = decltype(check(std::declval<T*>()));
};

template <typename T>
struct is_reverse_iterable final
{
    template <typename U, typename Iterator, typename ConstIterator>
    static std::true_type check(reverse_iterable<U, Iterator, ConstIterator>*);

    static std::false_type check(...);

    using result = decltype(check(std::declval<T*>()));
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

} // stream namespace
