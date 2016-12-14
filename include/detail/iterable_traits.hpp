#pragma once

namespace stream {

template <typename T, typename Iterator, typename ConstIterator>
class iterable;

template <typename T>
struct iterable_traits
{
};

template <typename T, typename Iterator, typename ConstIterator>
struct iterable_traits<iterable<T, Iterator, ConstIterator>>
{
    using value_type = T;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
};

} // stream namespace
