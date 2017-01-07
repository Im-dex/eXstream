#pragma once

#include "transform_iterator.hpp"
#include "option.hpp"

namespace cppstream {
namespace detail {
namespace flat_map {

class iterator_tag final {};

}} // detail::flat_map namespace

template <typename Iterator>
using end_flat_map_iterator = transform_iterator<Iterator, detail::flat_map::iterator_tag>;

template <typename Iterator,
          typename Function>
class begin_flat_map_iterator final : public transform_iterator<Iterator, detail::flat_map::iterator_tag>
{
    using stream_type = std::result_of_t<Function(decltype(*std::declval<Iterator>()))>;
    using stream_iterator_type = decltype(std::declval<stream_type>().begin());
public:

    explicit begin_flat_map_iterator(const Iterator& iterator, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    explicit begin_flat_map_iterator(Iterator&& iterator, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    begin_flat_map_iterator(const begin_flat_map_iterator&) = delete;
    begin_flat_map_iterator(begin_flat_map_iterator&&) = default;

    begin_flat_map_iterator& operator= (const begin_flat_map_iterator&) = delete;
    begin_flat_map_iterator& operator= (begin_flat_map_iterator&&) = delete;

    bool operator== (const begin_flat_map_iterator& that) noexcept(is_nothrow_comparable_v<Iterator> && is_nothrow_comparable_v<stream_iterator_type>)
    {
        return (iterator == that.iterator) && (stream_iterator == that.stream_iterator);
    }

    bool operator!= (const begin_flat_map_iterator& that) noexcept(is_nothrow_comparable_v<Iterator> && is_nothrow_comparable_v<stream_iterator_type>)
    {
        return (iterator != that.iterator) || (stream_iterator != that.stream_iterator);
    }

    template <typename ThatIterator>
    bool operator== (const end_flat_map_iterator<ThatIterator>& that) noexcept(is_nothrow_comparable_to_v<Iterator, Iterator> &&
                                                                               noexcept(std::declval<stream_type>().end()))
    {
        return (iterator == that.iterator) && (stream.empty() || stream_iterator == stream.get().end());
    }

    template <typename ThatIterator>
    bool operator!= (const end_flat_map_iterator<Iterator>& that) noexcept(is_nothrow_comparable_to_v<Iterator, ThatIterator> &&
                                                                           noexcept(std::declval<stream_type>().end()))
    {
        return (iterator != that.iterator) || (stream.non_empty() && stream_iterator != stream.get().end());
    }

    begin_flat_map_iterator& operator++ () noexcept(noexcept(++std::declval<Iterator>())          &&
                                                    std::is_nothrow_destructible_v<stream_type>   &&
                                                    is_nothrow_comparable_v<stream_iterator_type> &&
                                                    noexcept(++std::declval<stream_iterator_type>()))
    {
        if (stream.non_empty() && (stream_iterator != stream.get().end()))
        {
            ++stream_iterator;
        }
        else
        {
            stream.reset();
            ++iterator;
        }

        return *this;
    }

    // TODO: value_type<std::remove_reference_t<T>>
    auto operator* () noexcept(noexcept(std::declval<Function>()(*std::declval<Iterator>())) &&
                               noexcept(std::declval<stream_type>().begin())                 &&
                               noexcept(*(std::declval<stream_iterator_type>())))
    {
        if (stream.empty())
        {
            stream = function(*iterator);
            stream_iterator = stream.get().begin();
        }

        return *stream_iterator;
    }

private:

    const Function& function;
    stream_iterator_type stream_iterator;
    option<stream_type> stream;
};

template <typename XIterator, typename YIterator, typename Function>
bool operator== (const end_flat_map_iterator<XIterator>& x, const begin_flat_map_iterator<YIterator, Function>& y) noexcept(noexcept(y == x))
{
    return y == x;
}

template <typename XIterator, typename YIterator, typename Function>
bool operator!= (const end_flat_map_iterator<XIterator>& x, const begin_flat_map_iterator<YIterator, Function>& y) noexcept(noexcept(y != x))
{
    return y != x;
}

} // cppstream namespace
