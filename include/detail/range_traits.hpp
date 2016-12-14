#pragma once

#include "config.hpp"

STREAM_SUPRESS_ALL_WARNINGS
#include <iterator>
STREAM_RESTORE_ALL_WARNINGS

namespace stream {

template <typename T>
struct range_traits
{
    using begin_iterator = decltype(std::declval<T>().begin());
    using const_begin_iterator = decltype(std::declval<const T>().begin());

    using end_iterator = decltype(std::declval<T>().end());
    using const_end_iterator = decltype(std::declval<const T>().end());

    using value_type = typename std::iterator_traits<begin_iterator>::value_type;
};

} // stream namespace
