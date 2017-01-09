#pragma once

#include "transformations/with_transformations.hpp"
#include "transformations/transformation.hpp"

namespace cppstream {

template <typename T,
          typename Iterator,
          typename Allocator,
          typename Meta>
class stream final : public with_transformations<T, stream<T, Iterator, Allocator, Meta>>
{
public:

    using iterator_type = Iterator;
    using allocator = Allocator;
    using meta = Meta;

    explicit stream(Iterator&& iterator, const Allocator& alloc) noexcept(std::is_nothrow_move_constructible_v<Iterator> &&
                                                                          std::is_nothrow_copy_constructible_v<Allocator>)
        : alloc(alloc),
          iterator(iterator)
    {
    }

    explicit stream(Iterator&& iterator, Allocator&& alloc) noexcept(std::is_nothrow_move_constructible_v<Iterator> &&
                                                                     std::is_nothrow_move_constructible_v<Allocator>)
        : alloc(std::move(alloc)),
          iterator(std::move(iterator))
    {
    }

    stream(stream&&) = default; // TODO: hide from user

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    Iterator& get_iterator() noexcept
    {
        return iterator;
    }

    const Iterator& get_iterator() const noexcept
    {
        return iterator;
    }

    const Allocator& get_allocator() const noexcept
    {
        return alloc;
    }

private:

    const Allocator alloc;
    Iterator iterator;
};

} // cppstream namespace
