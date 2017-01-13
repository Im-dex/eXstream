#pragma once

#include "detail/type_traits.hpp"
#include "meta_info.hpp"
#include "transform_iterator.hpp"
#include "option.hpp"
#include "detail/result_traits.hpp"
#include "detail/scope_guard.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <unordered_set>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {

// TODO: possibility to provide comparator
// TODO: possibility to provide hash
template <typename Iterator,
          typename Meta,
          typename Allocator>
class distinct_iterator final : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
    // TODO: maybe use simple set to preserve order???
    using meta = meta_info<false, true, Order::Ascending>; // TODO: custom comparator can change this

    explicit distinct_iterator(const Iterator& iterator, const Allocator& alloc)
        : transform_iterator(iterator),
          set(alloc),
          elementIter()
    {
    }

    explicit distinct_iterator(Iterator&& iterator, const Allocator& alloc)
        : transform_iterator(std::move(iterator)),
          set(alloc),
          elementIter()
    {
    }

    distinct_iterator(distinct_iterator&&) = default;

    distinct_iterator(const distinct_iterator&) = delete;
    distinct_iterator& operator= (const distinct_iterator&) = delete;

    bool has_next()
    {
        if (!has_element()) fetch();
        return iterator.has_next() || has_element();
    }

    reference next()
    {
        if (!has_element()) fetch();
        return elementIter->copy();
    }

    void skip()
    {
        fetch();
    }

private:

    using storage = typename result_traits<reference>::storage;
    using set_type = std::unordered_set<storage, std::hash<storage>, std::equal_to<storage>, Allocator>;
    using set_iterator = typename set_type::iterator;

    // TODO: forward size if possible and then reserve
    // TODO: set.reserve(???);

    void fetch()
    {
        elementIter = set.end();

        while (iterator.has_next())
        {
            auto insertResult = set.emplace(iterator.next());
            if (insertResult.second)
            {
                elementIter = insertResult.first;
                break;
            }
        }
    }

    bool has_element() const noexcept
    {
        return elementIter != set.end();
    }

    set_type set;
    set_iterator elementIter;
};

template <typename Iterator,
          typename Allocator,
          bool IsOrdered,
          Order AnOrder>
class distinct_iterator<Iterator, meta_info<IsOrdered, true /*IsDistinct*/, AnOrder>, Allocator> final : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
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

    bool has_next() noexcept(noexcept(std::declval<Iterator&>().has_next()))
    {
        return iterator.has_next();
    }

    reference next() noexcept(noexcept(std::declval<Iterator&>().next()))
    {
        return iterator.next();
    }

    void skip() noexcept(noexcept(std::declval<Iterator&>().skip()))
    {
        iterator.skip();
    }
};

namespace detail {
namespace distinct {

template <typename Iterator>
constexpr bool is_nothrow_fetch() noexcept
{
    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
    using storage = typename result_traits<reference>::storage;

    return noexcept(std::declval<Iterator&>().has_next()) &&
           noexcept(std::declval<Iterator&>().next())     &&
           is_nothrow_comparable_v<value_type>            &&
           noexcept(std::declval<option<storage>&>().emplace(std::declval<reference>()));
}

}} // detail::distinct namespace


template <typename Iterator,
          typename Allocator,
          Order AnOrder>
class distinct_iterator<Iterator, meta_info<true /*IsOrdered*/, false /*IsDistinct*/, AnOrder>, Allocator> : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
    using meta = meta_info<true, true, AnOrder>;

    explicit distinct_iterator(const Iterator& iterator, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          cache(),
          valid_cache(false)
    {
    }

    explicit distinct_iterator(Iterator&& iterator, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          cache(),
          valid_cache(false)
    {
    }

    distinct_iterator(distinct_iterator&&) = default;

    distinct_iterator(const distinct_iterator&) = delete;
    distinct_iterator& operator= (const distinct_iterator&) = delete;

    bool has_next() noexcept(detail::distinct::is_nothrow_fetch<Iterator>())
    {
        if (!cache_has_value()) fetch();
        return iterator.has_next() || cache_has_value();
    }

    reference next() noexcept(detail::distinct::is_nothrow_fetch<Iterator>())
    {
        if (!cache_has_value()) fetch();
        EXSTREAM_SCOPE_EXIT noexcept { invalidate_cache(); };
        return cache.get().release();
    }

    void skip() noexcept(detail::distinct::is_nothrow_fetch<Iterator>())
    {
        invalidate_cache();
        fetch();
    }

private:

    using storage = typename result_traits<reference>::storage;

    void fetch() noexcept(detail::distinct::is_nothrow_fetch<Iterator>())
    {
        if (cache.empty())
        {
            if (iterator.has_next())
            {
                cache.emplace(iterator.next());
                valid_cache = true;
            }
            return;
        }

        while (iterator.has_next())
        {
            auto&& value = iterator.next();
            if (cache.get() != std::as_const(get_lvalue_reference(value)))
            {
                cache.emplace(std::forward<decltype(value)>(value));
                valid_cache = true;
                break;
            }
        }
    }

    void cache_has_value() const noexcept
    {
        return cache.non_empty() && valid_cache;
    }

    void invalidate_cache() noexcept
    {
        valid_cache = false;
    }

    option<storage> cache;
    bool valid_cache;
};

} // exstream namespace
