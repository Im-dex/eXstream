#pragma once

#include "adaptor_collector.hpp"

namespace std {

template <typename T, typename Allocator>
class deque;

template <typename T, typename Container>
class stack;

} // std namespace

namespace exstream {

template <typename T, typename Container>
auto to_stack(std::stack<T, Container>&& stack)
{
    return adaptor_collector<T, Container, std::stack>(std::move(stack));
}

inline auto to_stack() noexcept
{
    return generic_adaptor_collector<std::deque, std::stack>();
}

} // exstream namespace
