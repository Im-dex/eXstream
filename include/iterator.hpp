#pragma once

#include "detail/type_traits.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {

template <typename BeginIterator, typename EndIterator>
class iterator final
{
    static_assert(is_comparable_to_v<BeginIterator, EndIterator>, "Iterators should be comparable");
public:

    using value_type = typename std::iterator_traits<BeginIterator>::value_type;
    using result_type = typename std::iterator_traits<BeginIterator>::reference;

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

    bool has_next() const noexcept(is_nothrow_comparable_to_v<const BeginIterator, const EndIterator>)
    {
        return beginIterator != endIterator;
    }

    result_type next() noexcept(noexcept(*(std::declval<BeginIterator&>()++)))
    {
        assert(has_next() && "Iterator is out of range");
        return *(beginIterator++);
    }

    void skip() noexcept(noexcept(++std::declval<BeginIterator&>()))
    {
        assert(has_next() && "Iterator is out of range");
        ++beginIterator;
    }

    size_t elements_count() const noexcept(!is_random_access_iterator_v<BeginIterator> ||
                                           !is_random_access_iterator_v<EndIterator>)
    {
        return constexpr_if<is_random_access_iterator_v<BeginIterator> &&
                            is_random_access_iterator_v<EndIterator>>()
            .then([this](auto)
            {
                return static_cast<size_t>(std::distance(beginIterator, endIterator));
            })
            .else_([](auto) noexcept
            {
                return unknown_count;
            })(nothing);
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
} // exstream namespace
