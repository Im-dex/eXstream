#pragma once

namespace std {

template <typename Key, typename T, typename Compare, typename Alloc>
class map;

template <typename T, typename Compare, typename Alloc>
class set;

} // std namespace

namespace stream {

template <typename T>
struct ordering_traits final
{
    static constexpr bool is_ordered = false;
};

template <typename Key, typename T, typename Compare, typename Alloc>
struct ordering_traits<std::map<Key, T, Compare, Alloc>>
{
    static constexpr bool is_ordered = true;
};

template <typename T, typename Compare, typename Alloc>
struct ordering_traits<std::set<T, Compare, Alloc>>
{
    static constexpr bool is_ordered = true;
};

} // stream namespace
