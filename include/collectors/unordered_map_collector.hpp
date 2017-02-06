#pragma once

#include "detail/traits.hpp"

namespace std {

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator>
class unordered_map;

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator>
class unordered_multimap;

} // std namespace

namespace exstream {

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename, typename, typename> class Map>
class unordered_map_builder final
{
    using map_t = Map<Key, Value, Hash, Compare, Allocator>;
public:

    unordered_map_builder() = default;

    explicit unordered_map_builder(map_t&& map)
        : map(std::move(map))
    {
    }

    unordered_map_builder(unordered_map_builder&&) = default;

    unordered_map_builder(const unordered_map_builder&) = delete;
    unordered_map_builder& operator= (const unordered_map_builder&) = delete;

    void reserve(const size_t size)
    {
        map.reserve(size);
    }

    void append(const std::pair<Key, Value>& entry)
    {
        map.insert(entry);
    }

    void append(std::pair<Key, Value>&& entry)
    {
        map.insert(std::move(entry));
    }

    void append(const std::tuple<Key, Value>& entry)
    {
        map.insert(std::make_pair(std::get<0>(entry), std::get<1>(entry)));
    }

    void append(std::tuple<Key, Value>&& entry)
    {
        map.insert(std::make_pair(std::get<0>(std::move(entry)), std::get<1>(std::move(entry))));
    }

    map_t build()
    {
        return std::move(map);
    }

private:

    map_t map;
};

template <template <typename, typename, typename, typename, typename> class Map>
struct generic_unordered_map_collector final
{
    generic_unordered_map_collector() noexcept = default;
    generic_unordered_map_collector(generic_unordered_map_collector&&) noexcept = default;

    generic_unordered_map_collector(const generic_unordered_map_collector&) = delete;
    generic_unordered_map_collector& operator= (const generic_unordered_map_collector&) = delete;

    template <typename T, typename = std::enable_if_t<is_any_pair_v<T>>>
    auto builder(type_t<T>)
    {
        using first_t = std::tuple_element_t<0, T>;
        using second_t = std::tuple_element_t<1, T>;

        return unordered_map_builder<first_t, second_t, std::hash<T>, std::less<T>, std::allocator<T>, Map>();
    }
};

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename, typename, typename> class Map>
class unordered_map_collector final
{
    using map_t = Map<Key, Value, Hash, Compare, Allocator>;
public:

    explicit unordered_map_collector(map_t&& map)
        : map(std::move(map))
    {
    }

    unordered_map_collector(unordered_map_collector&&) = default;

    unordered_map_collector(const unordered_map_collector&) = delete;
    unordered_map_collector& operator= (const unordered_map_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>) -> std::enable_if_t<is_exact_pair_v<T, Key, Value>,
                                                unordered_map_builder<Key, Value, Hash, Compare, Allocator, Map>>
    {
        return unordered_map_builder<Key, Value, Hash, Compare, Allocator, Map>(std::move(map));
    }

private:

    map_t map;
};

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator>
auto to_unordered_map(std::unordered_map<Key, Value, Hash, Compare, Allocator>&& map)
{
    return unordered_map_collector<Key, Value, Hash, Compare, Allocator, std::unordered_map>(std::move(map));
}

inline auto to_unordered_map() noexcept
{
    return generic_unordered_map_collector<std::unordered_map>();
}

template <typename Key,
          typename Value,
          typename Hash,
          typename Compare,
          typename Allocator>
auto to_unordered_multimap(std::unordered_multimap<Key, Value, Hash, Compare, Allocator>&& map)
{
    return unordered_map_collector<Key, Value, Hash, Compare, Allocator, std::unordered_multimap>(std::move(map));
}

inline auto to_unordered_multimap() noexcept
{
    return generic_unordered_map_collector<std::unordered_multimap>();
}

} // exstream namespace
