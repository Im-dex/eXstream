#pragma once

#include "transform_iterator.hpp"

namespace cppstream {
namespace detail {
namespace map {

struct iterator_tag final {};

}} // detail::map namespace

template <typename Iterator,
          typename Function>
class begin_map_iterator final : public transform_iterator<Iterator, detail::map::iterator_tag>
{
public:

    explicit begin_map_iterator(const Iterator& iterator, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    explicit begin_map_iterator(Iterator&& iterator, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    begin_map_iterator(const begin_map_iterator&) = delete;
    begin_map_iterator(begin_map_iterator&&) = default;

    begin_map_iterator& operator= (const begin_map_iterator&) = delete;
    begin_map_iterator& operator= (begin_map_iterator&&) = delete;

    void operator++ () noexcept(noexcept(++std::declval<Iterator>()))
    {
        ++iterator;
    }

    // TODO: replace reference to primitive type with a value type
    decltype(auto) operator* () noexcept(noexcept(std::declval<Function>()(*std::declval<Iterator>())))
    {
        return function(*iterator);
    }

private:

    const Function& function;
};

template <typename Iterator>
using end_map_iterator = end_transform_iterator<Iterator, detail::map::iterator_tag>;

} // cppstream namespace
