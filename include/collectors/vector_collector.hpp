#pragma once

#include "sequence_collector.hpp"

namespace std {

template <typename T, typename Allocator>
class vector;

} // std namespace

namespace exstream {

template <typename T, typename Allocator>
auto to_vector(std::vector<T, Allocator>&& vector) noexcept(std::is_nothrow_constructible_v<sequence_collector<T, Allocator, std::vector>, std::vector<T, Allocator>&&> &&
                                                            std::is_nothrow_move_constructible_v<sequence_collector<T, Allocator, std::vector>>)
{
    return sequence_collector<T, Allocator, std::vector>(std::move(vector));
}

inline auto to_vector() noexcept
{
    return generic_sequence_collector<std::vector>();
}

} // exstream namespace
