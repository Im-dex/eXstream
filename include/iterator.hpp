#pragma once

#include "detail/type_traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <typename BeginIterator, typename EndIterator>
class iterator final
{
    static_assert(std::is_default_constructible_v<BeginIterator>, "Begin iterator should be default constructible");
    static_assert(std::is_default_constructible_v<EndIterator>, "End iterator should be default constructible");
    static_assert(is_comparable_to_v<BeginIterator, EndIterator>, "Iterators should be comparable");
public:

    using value_type = typename std::iterator_traits<BeginIterator>::value_type;
    using reference = typename std::iterator_traits<BeginIterator>::reference;

    iterator() noexcept(std::is_nothrow_default_constructible_v<BeginIterator> && std::is_nothrow_default_constructible_v<EndIterator>)
        : beginIterator(),
          endIterator()
    {
    }

    iterator(const BeginIterator& beginIterator, const EndIterator& endIterator) noexcept(std::is_nothrow_copy_constructible_v<BeginIterator> &&
                                                                                          std::is_nothrow_copy_constructible_v<EndIterator>)
        : beginIterator(beginIterator),
          endIterator(endIterator)
    {
    }

    iterator(BeginIterator&& beginIterator, EndIterator&& endIterator) noexcept(std::is_nothrow_move_constructible_v<BeginIterator> &&
                                                                                std::is_nothrow_move_constructible_v<EndIterator>)
        : beginIterator(std::move(beginIterator)),
          endIterator(std::move(endIterator))
    {
    }

    iterator(const iterator&) = default;
    iterator(iterator&&) = default;

    iterator& operator= (const iterator&) = delete;
    iterator& operator= (iterator&&) = delete;

    bool at_end() const noexcept(is_nothrow_comparable_to_v<const BeginIterator, const EndIterator>)
    {
        return beginIterator == endIterator;
    }

    void advance() noexcept(noexcept(++std::declval<BeginIterator>()))
    {
        assert(!at_end() && "Iterator is out of range");
        ++beginIterator;
    }

    reference get_value() noexcept(noexcept(*std::declval<BeginIterator>()))
    {
        assert(!at_end() && "Iterator is out of range");
        return *beginIterator;
    }

private:

    BeginIterator beginIterator;
    EndIterator endIterator;
};

namespace detail {

template <typename BeginIterator, typename EndIterator>
using iterator_type = iterator<std::decay_t<BeginIterator>, std::decay_t<EndIterator>>;

template <typename BeginIterator, typename EndIterator>
auto make_iterator(BeginIterator&& begin, EndIterator&& end)
    noexcept(std::is_nothrow_constructible_v<iterator_type<BeginIterator, EndIterator>, BeginIterator, EndIterator>)
{
    return iterator_type<BeginIterator, EndIterator>(std::forward<BeginIterator>(begin), std::forward<EndIterator>(end));
}

} // detail namespace
} // cppstream namespace
