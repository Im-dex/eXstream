#pragma once

#include "meta_info.hpp"
#include "transform_range.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <unordered_set>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

// TODO: possibility to provide comparator
// TODO: possibility to provide hash
template <typename Range,
          typename Meta,
          typename Allocator>
class distinct_range final : public transform_range<Range>
{
public:

    using value_type = typename Range::value_type;
    // TODO: maybe use simple set to preserve order???
    using meta = meta_info<false, true, Order::Ascending>; // TODO: custom comparator can change this

private:

    using set_value = remove_cvr_t<value_type>;
    using set_type = std::unordered_set<set_value, std::hash<set_value>, std::equal_to<set_value>, Allocator>;
    using set_iterator = typename set_type::iterator;

public:

    explicit distinct_range(const Range& range, const Allocator& alloc)
        : transform_range(range),
          set(0, alloc),
          iterator(),
          selected(false)
    {
    }

    explicit distinct_range(Range&& range, const Allocator& alloc)
        : transform_range(std::move(range)),
          set(0, alloc),
          iterator(),
          selected(false)
    {
    }

    distinct_range(distinct_range&&) = default;

    distinct_range(const distinct_range&) = delete;
    distinct_range& operator= (const distinct_range&) = delete;

    bool at_end() // TODO: const?
    {
        select();
        return iterator == std::end(set);
    }

    void advance()
    {
        select();
        ++iterator;
    }

    value_type get_value()
    {
        select();
        return std::move(*iterator);
    }

private:

    void select()
    {
        if (selected) return;

        // TODO: forward size if possible and then reserve
        // TODO: set.reserve(???);

        while (!range.at_end())
        {
            set.insert(range.get_value());
        }

        iterator = std::begin(set);
        selected = true;
    }

    set_type set;
    set_iterator iterator;
    bool selected;
};

template <typename Range,
          typename Allocator,
          bool IsOrdered,
          Order AnOrder>
class distinct_range<Range, meta_info<IsOrdered, true /*IsDistinct*/, AnOrder>, Allocator> final : public transform_range<Range>
{
public:

    using value_type = typename Range::value_type;
    using meta = meta_info<IsOrdered, true, AnOrder>;

    explicit distinct_range(const Range& range, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : transform_range(range)
    {
    }

    explicit distinct_range(Range&& range, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : transform_range(std::move(range))
    {
    }

    distinct_range(distinct_range&&) = default;

    distinct_range(const distinct_range&) = delete;
    distinct_range& operator= (const distinct_range&) = delete;

    bool at_end() /*TODO: const*/ noexcept(noexcept(std::declval<Range>().at_end()))
    {
        return range.at_end();
    }

    void advance() noexcept(noexcept(std::declval<Range>().advance()))
    {
        range.advance();
    }

    value_type get_value() noexcept(noexcept(std::declval<Range>().get_value()))
    {
        return range.get_value();
    }
};

namespace detail {
namespace distinct {

template <typename Range>
constexpr bool is_nothrow_find_next() noexcept
{
    using value_type = typename Range::value_type;
    using get_value_type = decltype(std::declval<Range>().get_value());

    return noexcept(std::declval<Range>().at_end())                    &&
           noexcept(std::declval<Range>().get_value())                 &&
           std::is_nothrow_constructible_v<value_type, get_value_type> &&
           is_nothrow_comparable_v<value_type>;
}

}} // detail::distinct namespace


template <typename Range,
          typename Allocator,
          Order AnOrder>
class distinct_range<Range, meta_info<true /*IsOrdered*/, false /*IsDistinct*/, AnOrder>, Allocator> : public transform_range<Range>
{
public:

    using value_type = typename Range::value_type;
    using meta = meta_info<true, true, AnOrder>;

    explicit distinct_range(const Range& range, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : transform_range(range),
          value()
    {
    }

    explicit distinct_range(Range&& range, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : transform_range(std::move(range)),
          value()
    {
    }

    distinct_range(distinct_range&&) = default;

    distinct_range(const distinct_range&) = delete;
    distinct_range& operator= (const distinct_range&) = delete;

    bool at_end() /*TODO: const*/ noexcept(detail::distinct::is_nothrow_find_next<Range>())
    {
        find_next();
        return range.at_end();
    }

    void advance() noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        value.reset();
    }

    value_type get_value() noexcept(detail::distinct::is_nothrow_find_next<Range>())
    {
        find_next();
        return std::move(value).get();
    }

private:

    void find_next() noexcept(detail::distinct::is_nothrow_find_next<Range>())
    {
        if (value.non_empty()) return;

        if (!range.at_end())
        {
            value.emplace(range.get_value());
            range.advance();
        }

        while (!range.at_end())
        {
            if (value != range.get_value()) break;
        }
    }

    option<value_type> value;
};

} // cppstream namespace
