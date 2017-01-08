#pragma once

#include "transformations/with_transformations.hpp"
#include "transformations/transformation.hpp"

namespace cppstream {

template <typename T, typename Range>
class stream final : public with_transformations<T, stream<T, Range>>
{
public:

    using range_type = Range;

    explicit stream(const Range& range) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : range(range)
    {
    }

    explicit stream(Range&& range) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : range(std::move(range))
    {
    }

    stream(stream&&) = default; // TODO: hide from user

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    const Range& get_range() const noexcept
    {
        return range;
    }

private:

    Range range;
};

} // cppstream namespace
