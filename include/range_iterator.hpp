#pragma once

#include "iterator.hpp"

namespace stream {

template <typename T,
          typename BeginIterator,
          typename EndIterator,
          bool Const>
class basic_range_iterator final : public iterator<T, Const>
{
public:

    explicit basic_range_iterator(const BeginIterator& begin, const EndIterator& end) noexcept(std::is_nothrow_copy_constructible_v<BeginIterator> &&
                                                                                               std::is_nothrow_copy_constructible_v<EndIterator>)
        : begin(begin),
          end(end)
    {
    }

    explicit basic_range_iterator(BeginIterator&& begin, EndIterator&& end) noexcept(std::is_nothrow_move_constructible_v<BeginIterator> &&
                                                                                     std::is_nothrow_move_constructible_v<EndIterator>)
        : begin(std::move(begin)),
          end(std::move(end))
    {
    }

    explicit basic_range_iterator(const BeginIterator& begin, EndIterator&& end) noexcept(std::is_nothrow_copy_constructible_v<BeginIterator> &&
                                                                                          std::is_nothrow_move_constructible_v<EndIterator>)
        : begin(begin),
          end(std::move(end))
    {
    }

    explicit basic_range_iterator(BeginIterator&& begin, const EndIterator& end) noexcept(std::is_nothrow_move_constructible_v<BeginIterator> &&
                                                                                          std::is_nothrow_copy_constructible_v<EndIterator>)
        : begin(std::move(begin)),
          end(end)
    {
    }

    bool operator== (const basic_range_iterator& that) const noexcept(noexcept(BeginIterator::operator==) && noexcept(EndIterator::operator==))
    {
        return (begin == that.begin) && (end == that.end);
    }

    bool operator!= (const basic_range_iterator& that) const noexcept(noexcept(BeginIterator::operator==) && noexcept(EndIterator::operator==))
    {
        return !(*this == that);
    }

    bool is_empty() const noexcept(is_nothrow_comparable_to_v<BeginIterator, EndIterator>) override
    {
        return begin == end;
    }

    bool non_empty() const noexcept(is_nothrow_comparable_to_v<BeginIterator, EndIterator>) override
    {
        return !(begin == end);
    }

    std::conditional_t<Const, const T&, T&> value() noexcept(noexcept(*std::declval<BeginIterator>())) override
    {
        return *begin;
    }

    const T& value() const noexcept(noexcept(*std::declval<const BeginIterator>())) override
    {
        return *begin;
    }

    void advance() noexcept(noexcept(++std::declval<BeginIterator>())) override
    {
        ++begin;
    }

private:

    BeginIterator begin;
    EndIterator end;
};

template <typename T, typename BeginIterator, typename EndIterator>
using range_iterator = basic_range_iterator<T, BeginIterator, EndIterator, false>;

template <typename T, typename BeginIterator, typename EndIterator>
using const_range_iterator = basic_range_iterator<T, BeginIterator, EndIterator, true>;

} // stream namespace
