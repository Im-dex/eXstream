#pragma once

#include "config.hpp"
#include "utility.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace std {

template <typename T>
class allocator;

template <typename T, typename Compare, typename Allocator>
class set;

template <typename T, typename Compare, typename Allocator>
class multiset;

} // std namespace

namespace exstream {

template <typename T,
          template <typename, typename, typename> class Set,
          typename Compare,
          typename Allocator>
class set_builder final
{
    using set_t = Set<T, Compare, Allocator>;
public:

    set_builder() = default;

    explicit set_builder(set_t&& set)
        : set(std::move(set))
    {
    }

    set_builder(set_builder&&) = default;

    set_builder(const set_builder&) = delete;
    set_builder& operator= (const set_builder&) = delete;

    void reserve(const size_t) const noexcept
    {
    }

    void append(const T& value)
    {
        set.insert(value);
    }

    void append(T&& value)
    {
        set.insert(std::move(value));
    }

    set_t build()
    {
        return std::move(set);
    }

private:

    set_t set;
};

template <template <typename, typename, typename> class Set>
struct generic_set_collector final
{
    generic_set_collector() noexcept = default;
    generic_set_collector(generic_set_collector&&) noexcept = default;

    generic_set_collector(const generic_set_collector&) = delete;
    generic_set_collector& operator= (const generic_set_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>)
    {
        return set_builder<T, Set, std::less<T>, std::allocator<T>>();
    }
};

template <typename Key,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename> class Set>
class set_collector final
{
    using set_t = Set<Key, Compare, Allocator>;
public:

    explicit set_collector(set_t&& set)
        : set(std::move(set))
    {
    }

    set_collector(set_collector&&) = default;

    set_collector(const set_collector&) = delete;
    set_collector& operator= (const set_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>) -> std::enable_if_t<std::is_same_v<T, Key>, set_builder<T, Set, Compare, Allocator>>
    {
        return set_builder<T, Set, Compare, Allocator>(std::move(set));
    }

private:

    set_t set;
};

template <typename Key, typename Compare, typename Allocator>
auto to_set(std::set<Key, Compare, Allocator>&& set)
{
    return set_collector<Key, Compare, Allocator, std::set>(std::move(set));
}

inline auto to_set() noexcept
{
    return generic_set_collector<std::set>();
}

template <typename Key, typename Compare, typename Allocator>
auto to_multiset(std::multiset<Key, Compare, Allocator>&& set)
{
    return set_collector<Key, Compare, Allocator, std::multiset>(std::move(set));
}

inline auto to_multiset() noexcept
{
    return generic_set_collector<std::multiset>();
}

} // exstream namespace
