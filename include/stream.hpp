#pragma once

#include "transformations/with_transformations.hpp"
#include "transformations/transformation.hpp"

namespace cppstream {

template <typename T,
          typename Range,
          typename Allocator,
          typename Meta>
class stream final : public with_transformations<T, stream<T, Range, Allocator, Meta>>
{
public:

    using range_type = Range;
    using allocator = Allocator;
    using meta = Meta;

    explicit stream(Range&& range, const Allocator& alloc) noexcept(std::is_nothrow_move_constructible_v<Range> &&
                                                                    std::is_nothrow_copy_constructible_v<Allocator>)
        : alloc(alloc),
          range(range)
    {
    }

    explicit stream(Range&& range, Allocator&& alloc) noexcept(std::is_nothrow_move_constructible_v<Range> &&
                                                               std::is_nothrow_move_constructible_v<Allocator>)
        : alloc(std::move(alloc)),
          range(std::move(range))
    {
    }

    stream(stream&&) = default; // TODO: hide from user

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    Range& get_range() noexcept
    {
        return range;
    }

    const Range& get_range() const noexcept
    {
        return range;
    }

    const Allocator& get_allocator() const noexcept
    {
        return alloc;
    }

private:

    const Allocator alloc;
    Range range;
};

} // cppstream namespace
