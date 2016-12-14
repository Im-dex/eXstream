#pragma once

#include "iterator.hpp"

namespace stream {

template <typename T,
          typename Iterator,
          typename ConstIterator>
class STREAM_NO_VTABLE iterable
{
    static_assert(std::is_base_of_v<iterator<T, false>, Iterator>, "Iterator should be inherited from stream::iterator<T, false>");
    static_assert(std::is_base_of_v<iterator<T, true>, ConstIterator>, "ConstIterator should be inherited from stream::iterator<T, true>");
public:

    virtual Iterator iterator() = 0;

    virtual ConstIterator iterator() const = 0;

    virtual ConstIterator const_iterator() = 0;
};

template <typename T,
          typename ReverseIterator,
          typename ConstReverseIterator>
class STREAM_NO_VTABLE reverse_iterable
{
    static_assert(std::is_base_of_v<iterator<T, false>, ReverseIterator>, "ReverseIterator should be inherited from stream::iterator<T, false>");
    static_assert(std::is_base_of_v<iterator<T, true>, ConstReverseIterator>, "ConstReverseIterator should be inherited from stream::iterator<T, true>");
public:

    virtual ReverseIterator reverse_iterator() = 0;

    virtual ConstReverseIterator reverse_iterator() const = 0;

    virtual ConstReverseIterator const_reverse_iterator() = 0;
};

} // stream namespace
