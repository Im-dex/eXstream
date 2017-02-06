#pragma once

#include "detail/traits.hpp"

namespace std {

template <typename Key, typename Value, typename Compare, typename Allocator>
class map;

template <typename Key, typename Value, typename Compare, typename Allocator>
class multimap;

} // std namespace

namespace exstream {

template <typename Key,
          typename Value,
          template <typename, typename, typename, typename> class Map,
          typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<Key, Value>>>
class map_builder final
{
    using map_t = Map<Key, Value, Compare, Allocator>;
public:

    map_builder() = default;

    explicit map_builder(map_t&& map)
        : map(std::move(map))
    {
    }

    map_builder(map_builder&&) = default;

    map_builder(const map_builder&) = delete;
    map_builder& operator= (const map_builder&) = delete;

    void reserve(const size_t) const noexcept
    {
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

template <template <typename, typename, typename, typename> class Map>
struct generic_map_collector final
{
    generic_map_collector() noexcept = default;
    generic_map_collector(generic_map_collector&&) noexcept = default;

    generic_map_collector(const generic_map_collector&) = delete;
    generic_map_collector& operator= (const generic_map_collector&) = delete;

    template <typename T, typename = std::enable_if_t<is_any_pair_v<T>>>
    auto builder(type_t<T>)
    {
        using first_t = std::tuple_element_t<0, T>;
        using second_t = std::tuple_element_t<1, T>;

        return map_builder<first_t, second_t, Map>();
    }
};

template <typename Key,
          typename Value,
          typename Compare,
          typename Allocator,
          template <typename, typename, typename, typename> class Map>
class map_collector final
{
    using map_t = Map<Key, Value, Compare, Allocator>;
public:

    explicit map_collector(map_t&& map)
        : map(std::move(map))
    {
    }

    map_collector(map_collector&&) = default;

    map_collector(const map_collector&) = delete;
    map_collector& operator= (const map_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>) -> std::enable_if_t<is_exact_pair_v<T, Key, Value>, map_builder<Key, Value, Map, Compare, Allocator>>
    {
        return map_builder<Key, Value, Map, Compare, Allocator>(std::move(map));
    }

private:

    map_t map;
};

template <typename Key,
          typename Value,
          typename Compare,
          typename Allocator>
auto to_map(std::map<Key, Value, Compare, Allocator>&& map)
{
    return map_collector<Key, Value, Compare, Allocator, std::map>(std::move(map));
}

inline auto to_map() noexcept
{
    return generic_map_collector<std::map>();
}

template <typename Key,
          typename Value,
          typename Compare,
          typename Allocator>
auto to_multimap(std::multimap<Key, Value, Compare, Allocator>&& map)
{
    return map_collector<Key, Value, Compare, Allocator, std::multimap>(std::move(map));
}

inline auto to_multimap() noexcept
{
    return generic_map_collector<std::multimap>();
}

} // exstream namespace
