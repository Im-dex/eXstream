#pragma once

#include "detail/type_traits.hpp"
#include "constexpr_if.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <typename T, typename Iter>
class transform_iterator
{
public:

    explicit transform_iterator(const Iter& iterator) noexcept(std::is_nothrow_copy_constructible_v<Iter>)
        : iterator(iterator)
    {
    }

    explicit transform_iterator(Iter&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iter>)
        : iterator(std::move(iterator))
    {
    }

protected:

    template <typename U, typename XIter, typename YIter>
    friend bool operator== (const transform_iterator<U, XIter>& x, const YIter& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>);

    Iter iterator;
};

template <typename T, typename Iter>
class end_transform_iterator final : public transform_iterator<T, Iter>
{
public:

    explicit end_transform_iterator(const Iter& iterator) noexcept(std::is_nothrow_copy_constructible_v<Iter>)
        : transform_iterator(iterator)
    {
    }

    explicit end_transform_iterator(Iter&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iter>)
        : transform_iterator(std::move(iterator))
    {
    }

    end_transform_iterator& operator++ () noexcept
    {
        assert(false && "Iterator is empty");
        return *this;
    }

    // TODO: __declspec(noreturn)
    T operator* () noexcept
    {
        assert(false && "Iterator is empty");
        // TODO: terminate or smth else
        return *reinterpret_cast<T*>(this);
    }
};

#pragma region iterator comparison

template <typename T, typename XIter, typename YIter>
bool operator== (const transform_iterator<T, XIter>& x, const YIter& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return CPPSTREAM_CONSTEXPR_IFELSE((is_comparable_to_v<XIter, YIter>),
        noexcept(is_nothrow_comparable_to_v<XIter, YIter>) {
            return x.iterator == y;
        },
        noexcept {
            static_assert(false, "Iterators are not comparable");
            return false;
        }
    );
}

template <typename T, typename XIter, typename YIter>
bool operator== (const YIter& x, const transform_iterator<T, XIter>& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return y == x;
}

template <typename T, typename XIter, typename YIter>
bool operator!= (const YIter& x, const transform_iterator<T, XIter>& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return !(y == x);
}

template <typename T, typename XIter, typename YIter>
bool operator!= (const transform_iterator<T, XIter>& x, const YIter& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return !(x == y);
}

template <typename T, typename XIter, typename YIter>
bool operator== (const transform_iterator<T, XIter>& x, const transform_iterator<T, YIter>& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return x == y.iterator;
}

template <typename T, typename XIter, typename YIter>
bool operator!= (const transform_iterator<T, XIter>& x, const transform_iterator<T, YIter>& y) noexcept(is_nothrow_comparable_to_v<XIter, YIter>)
{
    return x != y.iterator;
}

#pragma endregion

} // cppstream namespace
