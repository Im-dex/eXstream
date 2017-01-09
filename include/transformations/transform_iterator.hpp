#pragma once

#include "config.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <typename Iterator>
class transform_iterator
{
public:

    explicit transform_iterator(const Iterator& iterator) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : iterator(iterator)
    {
    }

    explicit transform_iterator(Iterator&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : iterator(std::move(iterator))
    {
    }

    transform_iterator(const transform_iterator&) = delete;
    transform_iterator(transform_iterator&&) = default;

    transform_iterator& operator= (const transform_iterator&) = delete;
    transform_iterator& operator= (transform_iterator&&) = delete;

protected:

    Iterator iterator;
};

} // cppstream namespace
