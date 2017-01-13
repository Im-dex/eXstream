#pragma once

#include "transform_iterator.hpp"
#include "detail/result_traits.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {

template <typename Iterator,
          typename Function,
          typename Meta>
class map_iterator final : public transform_iterator<Iterator>
{
    using traits = result_traits<std::result_of_t<const Function&(typename Iterator::reference)>>;
public:

    using value_type = typename traits::type;
    using reference = typename traits::reference;
    using meta = Meta;

    template <typename Allocator>
    explicit map_iterator(const Iterator& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    template <typename Allocator>
    explicit map_iterator(Iterator&& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    map_iterator(const map_iterator&) = delete;
    map_iterator(map_iterator&&) = default;

    map_iterator& operator= (const map_iterator&) = delete;
    map_iterator& operator= (map_iterator&&) = delete;

    bool has_next() noexcept(noexcept(std::declval<Iterator&>().has_next()))
    {
        return iterator.has_next();
    }

    // TODO: replace reference to primitive type with a value type
    reference next() noexcept(noexcept(std::declval<const Function&>()(std::declval<Iterator&>().next())))
    {
        assert(has_next() && "Iterator is out of range");
        return traits::unwrap(function(iterator.next()));
    }

    void skip() noexcept(noexcept(std::declval<Iterator&>().skip()))
    {
        assert(has_next() && "Iterator is out of range");
        iterator.skip();
    }

private:

    const Function& function;
};

} // exstream namespace
