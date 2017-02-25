#pragma once

#include "transformations/transformation.hpp"

namespace exstream {

template <typename T,
          typename Iterator,
          typename Meta>
class stream final : public with_transformations<T, stream<T, Iterator, Meta>>,
                     public terminator<T, stream<T, Iterator, Meta>>
{
public:

    using iterator_type = Iterator;
    using meta = Meta;

    explicit stream(Iterator&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : iterator(iterator)
    {
    }

    stream(stream&&) = default;

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    Iterator get_iterator() const noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
    {
        return iterator;
    }

private:

    Iterator iterator;
};

namespace detail {

template <typename Meta, typename Iterator>
using stream_type = stream<
    typename std::decay_t<Iterator>::value_type,
    std::decay_t<Iterator>,
    Meta
>;

template <typename Meta, typename Iterator>
auto make_stream(Iterator&& iterator)
    noexcept(std::is_nothrow_constructible_v<stream_type<Meta, Iterator>, Iterator>)
{
    return stream_type<Meta, Iterator>(std::forward<Iterator>(iterator));
}


} // detail namespace
} // exstream namespace
