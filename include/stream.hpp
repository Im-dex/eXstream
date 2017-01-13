#pragma once

#include "transformations/with_transformations.hpp"
#include "transformations/transformation.hpp"

namespace exstream {

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

namespace detail {

template <typename Allocator, typename Meta, typename Iterator>
using stream_type = stream<
    typename std::decay_t<Iterator>::value_type,
    std::decay_t<Iterator>,
    Allocator,
    Meta
>;

template <typename Meta, typename Iterator, typename Allocator>
auto make_stream(Iterator&& iterator, const Allocator& alloc)
    noexcept(std::is_nothrow_constructible_v<stream_type<Allocator, Meta, Iterator>, Iterator, const Allocator&>)
{
    return stream_type<Allocator, Meta, Iterator>(std::forward<Iterator>(iterator), alloc);
}


} // detail namespace
} // exstream namespace
