#pragma once

#include "stream.hpp"
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
    using begin_iterator = decltype(std::declval<iterable>().begin());
    using end_iterator = decltype(std::declval<iterable>().end());

    static constexpr bool result = noexcept(std::declval<Iterable>().begin())           &&
                                   noexcept(std::declval<Iterable>().end())             &&
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

            using begin_iterator = decltype(ref.begin());
            using end_iterator = decltype(ref.end());
            using value_type = decltype(*std::declval<begin_iterator>());

            return stream<value_type, begin_iterator, end_iterator>(ref.begin(), ref.end());
        })
        .else_([](auto) noexcept
        {
            static_assert(false, "Stream source should meet 'Iterable' concept");
            return error_transformation();
        })(std::forward<T>(iterable));
}

} // cppstream namespace
