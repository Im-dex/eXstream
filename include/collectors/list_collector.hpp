#pragma once

#include "sequence_collector.hpp"

namespace std {

template <typename T, typename Allocator>
class list;

} // std namespace

namespace exstream {

template <typename T, typename Allocator>
auto to_list(std::list<T, Allocator>&& list)
{
    return sequence_collector<T, Allocator, std::list>(std::move(list));
}

inline auto to_list() noexcept
{
    return generic_sequence_collector<std::list>();
}

} // exstream namespace
