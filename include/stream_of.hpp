#pragma once

#include "stream.hpp"
#include "iterator.hpp"
#include "detail/traits.hpp"
#include "meta_info.hpp"

namespace exstream {
namespace stream_of_detail {

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
class build_meta final
{
    using traits = container_traits<remove_cvr_t<Iterable>>; // TODO: decay?
public:

    using type = meta_info<
        traits::is_ordered,
        traits::is_distinct,
        order_selector<traits, traits::is_ordered>::order
    >;
};

template <typename Iterable>
using build_meta_t = typename build_meta<Iterable>::type;

template <typename Iterable, typename Allocator, bool IsRvalueReference, bool IsIterable = false>
struct is_stream_nothrow_constructible final
{
    static constexpr bool result = false;
};

template <typename Iterable, typename Allocator>
struct is_stream_nothrow_constructible<Iterable, Allocator, true /*IsRvalueReference*/, true /*IsIterable*/> final
{
    static constexpr bool result = noexcept(
        detail::make_stream<build_meta<Iterable>>(
            detail::make_iterator(std::make_move_iterator(std::begin(std::declval<Iterable>())),
                                  std::make_move_iterator(std::end(std::declval<Iterable>()))),
            std::declval<const Allocator&>()
        )
    );
};

template <typename Iterable, typename Allocator>
struct is_stream_nothrow_constructible<Iterable, Allocator, false /*IsRvalueReference*/, true /*IsIterable*/> final
{
    static constexpr bool result = noexcept(
        detail::make_stream<build_meta<Iterable>>(
            detail::make_iterator(std::cbegin(std::declval<Iterable>()), std::cend(std::declval<Iterable>())),
            std::declval<const Allocator&>()
        )
    );
};

template <typename Iterable, typename Allocator>
constexpr bool is_stream_nothrow_constructible_v = is_stream_nothrow_constructible<Iterable, Allocator,
                                                                                   std::is_rvalue_reference_v<Iterable>,
                                                                                   is_iterable_v<remove_cvr_t<Iterable>>>::result;
} // stream_of_detail namespasce

template <typename Allocator = std::allocator<unsigned char> /* TODO: maybe take from iterable??? */,
          typename T>
auto stream_of(T&& iterable, const Allocator& alloc = Allocator()) noexcept(stream_of_detail::is_stream_nothrow_constructible_v<T, Allocator>)
{
    return constexpr_if<is_iterable_v<remove_cvr_t<T>>>()
        .then([](auto&& iterable, const auto& alloc) noexcept(stream_of_detail::is_stream_nothrow_constructible_v<decltype(iterable), std::decay_t<decltype(alloc)>>)
        {
            auto&& iterator = constexpr_if<std::is_rvalue_reference_v<decltype(iterable)>>()
                .then([&](auto) noexcept(noexcept(detail::make_iterator(std::make_move_iterator(std::begin(iterable)),
                                                                        std::make_move_iterator(std::end(iterable)))))
                {
                    return detail::make_iterator(std::make_move_iterator(std::begin(iterable)),
                                                 std::make_move_iterator(std::end(iterable)));
                })
                .else_([&](auto) noexcept(noexcept(detail::make_iterator(std::cbegin(iterable), std::cend(iterable))))
                {
                    return detail::make_iterator(std::cbegin(iterable), std::cend(iterable));
                })(nothing);

            using meta = stream_of_detail::build_meta_t<decltype(iterable)>;
            return detail::make_stream<meta>(std::forward<decltype(iterator)>(iterator), alloc);
        })
        .else_([](auto, auto) noexcept
        {
            static_assert(false, "Stream source should meet 'Iterable' concept");
            return error_transformation();
        })(std::forward<T>(iterable), alloc);
}

} // exstream namespace
