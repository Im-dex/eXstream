#pragma once

#include "config.hpp"
#include "utility.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace std {

template <typename T>
class allocator;

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator>
class unordered_set;

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator>
class unordered_multiset;

} // std namespace

namespace exstream {

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename, typename> class Set>
class unordered_set_builder final
{
    using set_t = Set<Key, Hash, Compare, Allocator>;
public:

    unordered_set_builder() = default;

    explicit unordered_set_builder(set_t&& set)
        : set(std::move(set))
    {
    }

    unordered_set_builder(unordered_set_builder&&) = default;

    unordered_set_builder(const unordered_set_builder&) = delete;
    unordered_set_builder& operator= (const unordered_set_builder&) = delete;

    void reserve(const size_t size)
    {
        set.reserve(size);
    }

    void append(const Key& key)
    {
        set.insert(key);
    }

    void append(Key&& key)
    {
        set.insert(std::move(key));
    }

    set_t build()
    {
        return std::move(set);
    }

private:

    set_t set;
};

template <template <typename, typename, typename, typename> class Set>
struct generic_unordered_set_collector final
{
    generic_unordered_set_collector() noexcept = default;
    generic_unordered_set_collector(generic_unordered_set_collector&&) noexcept = default;

    generic_unordered_set_collector(const generic_unordered_set_collector&) = delete;
    generic_unordered_set_collector& operator= (const generic_unordered_set_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>)
    {
        return unordered_set_builder<T, std::hash<T>, std::less<T>, std::allocator<T>, Set>();
    }
};

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename, typename> class Set>
class unordered_set_collector final
{
    using set_t = Set<Key, Hash, Compare, Allocator>;
public:

    explicit unordered_set_collector(set_t&& set)
        : set(std::move(set))
    {
    }

    unordered_set_collector(unordered_set_collector&&) = default;

    unordered_set_collector(const unordered_set_collector&) = delete;
    unordered_set_collector& operator= (const unordered_set_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>) -> std::enable_if_t<std::is_same_v<Key, T>,
                                                unordered_set_builder<T, Hash, Compare, Allocator, Set>>
    {
        return unordered_set_builder<T, Hash, Compare, Allocator, Set>(std::move(set));
    }

private:

    set_t set;
};

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator>
auto to_unordered_set(std::unordered_set<Key, Hash, Compare, Allocator>&& set)
{
    return unordered_set_collector<Key, Hash, Compare, Allocator, std::unordered_set>(std::move(set));
}

inline auto to_unordered_set() noexcept
{
    return generic_unordered_set_collector<std::unordered_set>();
}

template <typename Key,
          typename Hash,
          typename Compare,
          typename Allocator>
auto to_unordered_multiset(std::unordered_multiset<Key, Hash, Compare, Allocator>&& set)
{
    return unordered_set_collector<Key, Hash, Compare, Allocator, std::unordered_multiset>(std::move(set));
}

inline auto to_unordered_multiset() noexcept
{
    return generic_unordered_set_collector<std::unordered_multiset>();
}

} // exstream namespace
