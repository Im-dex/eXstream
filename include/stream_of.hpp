#pragma once

#include "stream.hpp"
#include "iterator.hpp"
#include "detail/traits.hpp"
#include "meta_info.hpp"

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

template <typename Traits, bool IsOrdered = false>
struct order_selector
{
    static constexpr Order order = Order::Unknown;
};

template <typename Traits>
struct order_selector<Traits, true>
{
    static constexpr Order order = Traits::order;
};

template <typename Iterable>
class meta_builder final
{
    using traits = container_traits<Iterable>;
public:

    using type = meta_info<
        traits::is_ordered,
        traits::is_distinct,
        order_selector<traits, traits::is_ordered>::order
    >;
};

}} // detail::stream_of namespasce

template <typename Allocator = std::allocator<unsigned char> /* TODO: maybe take from iterable??? */,
          typename T>
auto stream_of(T&& iterable, const Allocator& alloc = Allocator()) noexcept(detail::stream_of::is_stream_nothrow_constructible_v<T>)
{
    return constexpr_if<is_iterable_v<remove_cvr_t<T>>>()
        .then([](auto&& iterable, const auto& alloc) noexcept(detail::stream_of::is_stream_nothrow_constructible_v<decltype(iterable)>)
        {
            detail::stream_of::iterable_type<decltype(iterable)> ref = iterable;

            using begin_iterator = decltype(std::begin(ref));
            using end_iterator = decltype(std::end(ref));
            using value_type = decltype(*std::declval<begin_iterator>());
            using iterator_type = iterator<begin_iterator, end_iterator>;
            using meta = typename detail::stream_of::meta_builder<remove_cvr_t<decltype(iterable)>>::type;

            // TODO: move_iterator
            return stream<value_type, iterator_type, Allocator, meta>(iterator_type(std::begin(ref), std::end(ref)), alloc);
        })
        .else_([](auto, auto) noexcept
        {
            static_assert(false, "Stream source should meet 'Iterable' concept");
            return error_transformation();
        })(std::forward<T>(iterable), alloc);
}

} // cppstream namespace
