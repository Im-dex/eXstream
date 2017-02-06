#pragma once

#include "sequence_collector.hpp"

namespace std {

template <typename T, typename Allocator>
class deque;

} // std namespace

namespace exstream {

template <typename T, typename Allocator>
auto to_deque(std::deque<T, Allocator>&& deque)
{
    return sequence_collector<T, Allocator, std::deque>(std::move(deque));
}

inline auto to_deque() noexcept
{
    return generic_sequence_collector<std::deque>();
}

} // exstream namespace
