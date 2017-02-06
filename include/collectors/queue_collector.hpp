#pragma once

#include "adaptor_collector.hpp"

namespace std {

template <typename T, typename Allocator>
class deque;

template <typename T, typename Container>
class queue;

} // std namespace

namespace exstream {

template <typename T, typename Container>
auto to_queue(std::queue<T, Container>&& queue)
{
    return adaptor_collector<T, Container, std::queue>(std::move(queue));
}

inline auto to_queue() noexcept
{
    return generic_adaptor_collector<std::deque, std::queue>();
}

} // exstream namespace
