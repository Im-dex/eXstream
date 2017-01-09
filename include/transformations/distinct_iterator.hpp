#pragma once

#include "detail/type_traits.hpp"
#include "meta_info.hpp"
#include "transform_iterator.hpp"
#include "option.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <unordered_set>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

// TODO: possibility to provide comparator
// TODO: possibility to provide hash
template <typename Iterator,
          typename Meta,
          typename Allocator>
class distinct_iterator final : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    // TODO: maybe use simple set to preserve order???
    using meta = meta_info<false, true, Order::Ascending>; // TODO: custom comparator can change this

private:

    using set_value = remove_cvr_t<value_type>;
    using set_type = std::unordered_set<set_value, std::hash<set_value>, std::equal_to<set_value>, Allocator>;
    using set_iterator = typename set_type::iterator;

public:

    explicit distinct_iterator(const Iterator& iterator, const Allocator& alloc)
        : transform_iterator(iterator),
          set(0, alloc),
          iterator(),
          selected(false)
    {
    }

    explicit distinct_iterator(Iterator&& iterator, const Allocator& alloc)
        : transform_iterator(std::move(iterator)),
          set(0, alloc),
          iterator(),
          selected(false)
    {
    }

    distinct_iterator(distinct_iterator&&) = default;

    distinct_iterator(const distinct_iterator&) = delete;
    distinct_iterator& operator= (const distinct_iterator&) = delete;

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

template <typename Iterator,
          typename Allocator,
          bool IsOrdered,
          Order AnOrder>
class distinct_iterator<Iterator, meta_info<IsOrdered, true /*IsDistinct*/, AnOrder>, Allocator> final : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using meta = meta_info<IsOrdered, true, AnOrder>;

    explicit distinct_iterator(const Iterator& iterator, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator)
    {
    }

    explicit distinct_iterator(Iterator&& iterator, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator))
    {
    }

    distinct_iterator(distinct_iterator&&) = default;

    distinct_iterator(const distinct_iterator&) = delete;
    distinct_iterator& operator= (const distinct_iterator&) = delete;

    bool at_end() /*TODO: const*/ noexcept(noexcept(std::declval<Iterator>().at_end()))
    {
        return iterator.at_end();
    }

    void advance() noexcept(noexcept(std::declval<Iterator>().advance()))
    {
        iterator.advance();
    }

    value_type get_value() noexcept(noexcept(std::declval<Iterator>().get_value()))
    {
        return iterator.get_value();
    }
};

namespace detail {
namespace distinct {

template <typename Iterator>
constexpr bool is_nothrow_find_next() noexcept
{
    using value_type = typename Iterator::value_type;
    using get_value_type = decltype(std::declval<Iterator>().get_value());

    return noexcept(std::declval<Iterator>().at_end())                 &&
           noexcept(std::declval<Iterator>().get_value())              &&
           std::is_nothrow_constructible_v<value_type, get_value_type> &&
           is_nothrow_comparable_v<value_type>;
}

}} // detail::distinct namespace


template <typename Iterator,
          typename Allocator,
          Order AnOrder>
class distinct_iterator<Iterator, meta_info<true /*IsOrdered*/, false /*IsDistinct*/, AnOrder>, Allocator> : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using meta = meta_info<true, true, AnOrder>;

    explicit distinct_iterator(const Iterator& iterator, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          value()
    {
    }

    explicit distinct_iterator(Iterator&& iterator, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          value()
    {
    }

    distinct_iterator(distinct_iterator&&) = default;

    distinct_iterator(const distinct_iterator&) = delete;
    distinct_iterator& operator= (const distinct_iterator&) = delete;

    bool at_end() /*TODO: const*/ noexcept(detail::distinct::is_nothrow_find_next<Iterator>())
    {
        find_next();
        return iterator.at_end() && value.empty();
    }

    void advance() noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        value.reset();
    }

    value_type get_value() noexcept(detail::distinct::is_nothrow_find_next<Iterator>())
    {
        find_next();
        return std::move(value).get();
    }

private:

    void find_next() noexcept(detail::distinct::is_nothrow_find_next<Iterator>())
    {
        if (value.non_empty()) return;

        if (!iterator.at_end())
        {
            value.emplace(iterator.get_value());
            iterator.advance();
        }

        while (!iterator.at_end())
        {
            if (value != iterator.get_value()) break;
        }
    }

    option<value_type> value;
};

} // cppstream namespace
