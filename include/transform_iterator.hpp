#pragma once

#include "detail/type_traits.hpp"
#include "constexpr_if.hpp"

namespace cppstream {

template <typename Iterator,
          typename Tag = nothing_t /* since C++ does not provide strong typedef functionality, Tag should be used to express that purpose */>
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

    template <typename ThatIterator>
    bool operator== (const transform_iterator<ThatIterator, Tag>& that) const noexcept(is_nothrow_comparable_to_v<Iterator, ThatIterator>)
    {
        return constexpr_if<is_comparable_to_v<Iterator, ThatIterator>>()
            .then([&](auto) noexcept(is_nothrow_comparable_to_v<Iterator, ThatIterator>)
            {
                return iterator == that.iterator;
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Iterators are not comparable");
                return false;
            })(nothing);
    }

    template <typename ThatIterator>
    bool operator!= (const transform_iterator<ThatIterator, Tag>& that) const noexcept(is_nothrow_comparable_to_v<Iterator, ThatIterator>)
    {
        return !(*this == that);
    }

protected:

    Iterator iterator;
};

template <typename Iterator,
          typename Tag = nothing_t /* since C++ does not provide strong typedef functionality, Tag should be used to express that purpose */>
class end_transform_iterator final : public transform_iterator<Iterator, Tag>
{
public:

    explicit end_transform_iterator(const Iterator& iterator) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator)
    {
    }

    explicit end_transform_iterator(Iterator&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator))
    {
    }

    end_transform_iterator(const end_transform_iterator&) = delete;
    end_transform_iterator(end_transform_iterator&&) = default;

    end_transform_iterator& operator= (const end_transform_iterator&) = delete;
    end_transform_iterator& operator= (end_transform_iterator&&) = delete;

    void operator++ () = delete;
    void operator* () = delete;
};

} // cppstream namespace
