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

} // stream_of_detail namespasce

template <typename T>
auto stream_of(T&& iterable)
{
    return constexpr_if<is_iterable_v<remove_cvr_t<T>>>()
        .then([](auto&& iterable)
        {
            auto&& iterator = constexpr_if<std::is_rvalue_reference_v<decltype(iterable)>>()
                .then([&](auto)
                {
                    return detail::make_iterator(std::make_move_iterator(std::begin(iterable)),
                                                 std::make_move_iterator(std::end(iterable)));
                })
                .else_([&](auto)
                {
                    return detail::make_iterator(std::cbegin(iterable), std::cend(iterable));
                })(nothing);

            using meta = stream_of_detail::build_meta_t<decltype(iterable)>;
            return detail::make_stream<meta>(std::forward<decltype(iterator)>(iterator));
        })
        .else_([](auto, auto) noexcept
        {
            static_assert(false, "Stream source should meet 'Iterable' concept");
            return error_transformation();
        })(std::forward<T>(iterable));
}

template <typename T>
auto stream_of(std::initializer_list<T> list)
{
    using meta = meta_info<false, false, Order::Unknown>;

    auto&& iterator = detail::make_iterator(std::cbegin(list), std::cend(list));
    return detail::make_stream<meta>(std::move(iterator));
}

} // exstream namespace
