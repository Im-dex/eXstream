#pragma once

#include "config.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <typename Range>
class transform_range
{
public:

    explicit transform_range(const Range& range) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : range(range)
    {
    }

    explicit transform_range(Range&& range) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : range(std::move(range))
    {
    }

    transform_range(const transform_range&) = delete;
    transform_range(transform_range&&) = default;

    transform_range& operator= (const transform_range&) = delete;
    transform_range& operator= (transform_range&&) = delete;

protected:

    Range range;
};

} // cppstream namespace
