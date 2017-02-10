#pragma once

#include "config.hpp"
#include "utility.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace std {

template <typename T, typename Allocator>
class forward_list;

} // std namespace

namespace exstream {

template <typename T, typename Allocator>
class forward_list_builder final
{
    using list_t = std::forward_list<T, Allocator>;
    using iterator_t = typename list_t::iterator;
public:

    forward_list_builder()
        : list(),
          last(list.before_begin())
    {
    }

    explicit forward_list_builder(list_t&& list)
        : list(std::move(list)),
          last(list.before_begin())
    {
    }

    forward_list_builder(forward_list_builder&&) = default;

    forward_list_builder(const forward_list_builder&) = delete;
    forward_list_builder& operator= (const forward_list_builder&) = delete;

    void reserve(const size_t) const noexcept
    {
    }

    void append(const T& value)
    {
        last = list.insert_after(last, value);
    }

    void append(T&& value)
    {
        last = list.insert_after(last, std::move(value));
    }

    list_t build()
    {
        return std::move(list);
    }

private:

    list_t list;
    iterator_t last;
};

struct generic_forward_list_collector final
{
    generic_forward_list_collector() noexcept = default;
    generic_forward_list_collector(generic_forward_list_collector&&) noexcept = default;

    generic_forward_list_collector(const generic_forward_list_collector&) = delete;
    generic_forward_list_collector& operator= (const generic_forward_list_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>)
    {
        return forward_list_builder<T, std::allocator<T>>();
    }
};

template <typename T, typename Allocator>
class forward_list_collector final
{
    using list_t = std::forward_list<T, Allocator>;
public:

    explicit forward_list_collector(list_t&& list)
        : list(std::move(list))
    {
    }

    forward_list_collector(forward_list_collector&&) = default;

    forward_list_collector(const forward_list_collector&) = delete;
    forward_list_collector& operator= (const forward_list_collector&) = delete;

    template <typename U>
    auto builder(type_t<U>) -> std::enable_if_t<std::is_same_v<T, U>, forward_list_builder<U, Allocator>>
    {
        return forward_list_builder<U, Allocator>(std::move(list));
    }

private:

    list_t list;
};

template <typename T, typename Allocator>
auto to_forward_list(std::forward_list<T, Allocator>&& list)
{
    return forward_list_collector<T, Allocator>(std::move(list));
}

inline auto to_forward_list() noexcept
{
    return generic_forward_list_collector();
}

} // exstream namespace
