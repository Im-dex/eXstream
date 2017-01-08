#pragma once

#include "stream.hpp"
#include "range.hpp"
#include "detail/traits.hpp"

namespace cppstream {
namespace detail {
namespace stream_of {

template <typename Iterable>
using iterable_type = std::conditional_t<std::is_lvalue_reference_v<Iterable>,
                                         std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<Iterable>>>,
                                         std::add_lvalue_reference_t<std::remove_reference_t<Iterable>>>;

template <typename Iterable, bool IsIterable = false>
struct is_stream_nothrow_constructible final
{
    static constexpr bool result = false;
};

template <typename Iterable>
struct is_stream_nothrow_constructible<Iterable, true> final
{
    using iterable = iterable_type<Iterable>;
    using begin_iterator = decltype(std::begin(std::declval<iterable>()));
    using end_iterator = decltype(std::end(std::declval<iterable>()));

    static constexpr bool result = noexcept(std::begin(std::declval<Iterable>()))       &&
                                   noexcept(std::end(std::declval<Iterable>()))         &&
                                   std::is_nothrow_move_constructible_v<begin_iterator> &&
                                   std::is_nothrow_move_constructible_v<end_iterator>;
};

template <typename Iterable>
constexpr bool is_stream_nothrow_constructible_v = is_stream_nothrow_constructible<Iterable, is_iterable_v<remove_cvr_t<Iterable>>>::result;

}} // detail::stream_of namespasce

template <typename T>
auto stream_of(T&& iterable) noexcept(detail::stream_of::is_stream_nothrow_constructible_v<T>)
{
    return constexpr_if<is_iterable_v<remove_cvr_t<T>>>()
        .then([](auto&& iterable) noexcept(detail::stream_of::is_stream_nothrow_constructible_v<decltype(iterable)>)
        {
            detail::stream_of::iterable_type<decltype(iterable)> ref = iterable;

            using begin_iterator = decltype(std::begin(ref));
            using end_iterator = decltype(std::end(ref));
            using value_type = decltype(*std::declval<begin_iterator>());
            using range_type = range<begin_iterator, end_iterator>;

            return stream<value_type, range_type>(range_type(std::begin(ref), std::end(ref)));
        })
        .else_([](auto) noexcept
        {
            static_assert(false, "Stream source should meet 'Iterable' concept");
            return error_transformation();
        })(std::forward<T>(iterable));
}

} // cppstream namespace
