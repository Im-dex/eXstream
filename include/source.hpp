#pragma once

#include "stream.hpp"
#include "range_iterator.hpp"
#include "detail/iterable_traits.hpp"
#include "detail/range_traits.hpp"

namespace stream {
namespace detail {

template <typename T>
constexpr bool is_range_source_v = !is_iterable_v<remove_cvr_t<T>> && is_range_v<remove_cvr_t<T>>;

template <typename T>
constexpr bool is_iterable_source_v = is_iterable_v<remove_cvr_t<T>>;

template <typename T, bool IsRange, bool IsIterable>
struct source_traits_impl
{
    using stream_type = void;

    static constexpr bool is_nothrow_constructible() noexcept
    {
        return false;
    }
};

// range
template <typename T>
struct source_traits_impl<T, true, false>
{
    using traits = range_traits<T>;
    using value_type = typename traits::value_type;
    using iterator_type = const_range_iterator<value_type, typename traits::const_begin_iterator, typename traits::const_end_iterator>;
    using stream_type = stream<value_type, iterator_type>;

    static constexpr bool is_nothrow_constructible() noexcept
    {
        return std::is_nothrow_constructible_v<T, iterator_type&&>;
    }
};

// iterable
template <typename T>
struct source_traits_impl<T, false, true>
{
    using traits = iterable_traits<T>;
    using value_type = typename traits::value_type;
    using iterator_type = typename traits::const_iterator;
    using stream_type = stream<value_type, iterator_type>;

    static constexpr bool is_nothrow_constructible() noexcept
    {
        return std::is_nothrow_constructible_v<T, const iterator_type&>;
    }
};

// iterable
template <typename T>
struct source_traits_impl<T, true, true> : public source_traits_impl<T, false, true>
{
};

template <typename T>
using source_traits = source_traits_impl<remove_cvr_t<T>, is_range_source_v<T>, is_iterable_source_v<T>>;

} // detail namespace

template <typename T>
std::enable_if_t<detail::is_range_source_v<T>, typename detail::source_traits<T>::stream_type>
source(T&& streamable) noexcept(detail::source_traits<T>::is_nothrow_constructible())
{
    using traits = detail::source_traits<T>;
    using stream_type = typename traits::stream_type;
    using iterator = typename traits::iterator_type;

    const auto& constStreamable = streamable;
    return stream_type(iterator(constStreamable.begin(), constStreamable.end()));
}

template <typename T>
std::enable_if_t<detail::is_iterable_source_v<T>, typename detail::source_traits<T>::stream_type>
source(T&& streamable) noexcept(detail::source_traits<T>::is_nothrow_constructible())
{
    using traits = detail::source_traits<T>;
    using stream_type = typename traits::stream_type;

    return stream_type(streamable.const_iterator());
}

} // stream namespace
