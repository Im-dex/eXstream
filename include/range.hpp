#pragma once

#include "detail/type_traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <typename BeginIterator,
          typename EndIterator>
class range final
{
    static_assert(std::is_default_constructible_v<BeginIterator>, "Begin iterator should be default constructible");
    static_assert(std::is_default_constructible_v<EndIterator>, "End iterator should be default constructible");
    static_assert(is_comparable_to_v<BeginIterator, EndIterator>, "Iterators should be comparable");
public:

    using value_type = decltype(*std::declval<BeginIterator>());

    range() noexcept(std::is_nothrow_default_constructible_v<BeginIterator> && std::is_nothrow_default_constructible_v<EndIterator>)
        : beginIterator(),
          endIterator()
    {
    }

    range(const BeginIterator& beginIterator, const EndIterator& endIterator) noexcept(std::is_nothrow_copy_constructible_v<BeginIterator> &&
                                                                                       std::is_nothrow_copy_constructible_v<EndIterator>)
        : beginIterator(beginIterator),
          endIterator(endIterator)
    {
    }

    range(BeginIterator&& beginIterator, EndIterator&& endIterator) noexcept(std::is_nothrow_move_constructible_v<BeginIterator> &&
                                                                             std::is_nothrow_move_constructible_v<EndIterator>)
        : beginIterator(std::move(beginIterator)),
          endIterator(std::move(endIterator))
    {
    }

    range(const range&) = default;
    range(range&&) = default;

    range& operator= (const range&) = delete;
    range& operator= (range&&) = delete;

    bool at_end() const noexcept(is_nothrow_comparable_to_v<const BeginIterator, const EndIterator>)
    {
        return beginIterator == endIterator;
    }

    void advance() noexcept(noexcept(++std::declval<BeginIterator>()))
    {
        assert(!at_end() && "End of range");
        ++beginIterator;
    }

    value_type get_value() noexcept(noexcept(*std::declval<BeginIterator>()))
    {
        assert(!at_end() && "End of range");
        return *beginIterator;
    }

private:

    BeginIterator beginIterator;
    EndIterator endIterator;
};

} // cppstream namespace
