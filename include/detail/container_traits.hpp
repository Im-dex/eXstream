#pragma once

#include "compare_traits.hpp"

namespace std {

template <typename Key, typename T, typename Compare, typename Alloc>
class map;

template <typename Key, typename T, typename Compare, typename Alloc>
class multimap;

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
class unordered_map;

template <typename T, typename Compare, typename Alloc>
class set;

template <typename T, typename Compare, typename Alloc>
class multiset;

template <typename Key, typename Hash, typename KeyEqual, typename Allocator>
class unordered_set;

} // std namespace

namespace exstream {
namespace detail {
namespace container {

template <typename Compare>
struct ordered_container_traits
{
    static constexpr bool is_ordered = true;
    static constexpr Order order = compare_traits<Compare>::order;
};

}} // detail::container namespace

template <typename T>
struct container_traits final
{
    static constexpr bool is_ordered = false;
    static constexpr bool is_distinct = false;
};

template <typename Key, typename T, typename Compare, typename Alloc>
struct container_traits<std::map<Key, T, Compare, Alloc>> : public detail::container::ordered_container_traits<Compare>
{
    static constexpr bool is_distinct = true;
};

template <typename Key, typename T, typename Compare, typename Alloc>
struct container_traits<std::multimap<Key, T, Compare, Alloc>> : public detail::container::ordered_container_traits<Compare>
{
    static constexpr bool is_distinct = false;
};

template <typename Key, typename T, typename Hash, typename KeyCompare, typename Alloc>
struct container_traits<std::unordered_map<Key, T, Hash, KeyCompare, Alloc>>
{
    static constexpr bool is_ordered = false;
    static constexpr bool is_distinct = true;
};

template <typename T, typename Compare, typename Alloc>
struct container_traits<std::set<T, Compare, Alloc>> : public detail::container::ordered_container_traits<Compare>
{
    static constexpr bool is_distinct = true;
};

template <typename T, typename Compare, typename Alloc>
struct container_traits<std::multiset<T, Compare, Alloc>> : public detail::container::ordered_container_traits<Compare>
{
    static constexpr bool is_distinct = false;
};

template <typename Key, typename Hash, typename KeyCompare, typename Alloc>
struct container_traits<std::unordered_set<Key, Hash, KeyCompare, Alloc>>
{
    static constexpr bool is_ordered = false;
    static constexpr bool is_distinct = true;
};

} // exstream namespace
