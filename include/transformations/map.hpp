#pragma once

#include "transformation.hpp"
#include "transform_iterator.hpp"

namespace cppstream {

template <typename T,
          typename Iter,
          typename Function>
class begin_map_iterator final : public transform_iterator<T, Iter>
{
public:

    explicit begin_map_iterator(const Iter& iterator, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Iter>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    explicit begin_map_iterator(Iter&& iterator, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Iter>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    begin_map_iterator& operator++ () noexcept(noexcept(++std::declval<Iter>()))
    {
        ++iterator;
        return *this;
    }

    decltype(auto) operator* () noexcept(noexcept(std::declval<Function>()(*std::declval<Iter>())))
    {
        return function(*iterator);
    }

private:

    const Function& function;
};

template <typename T,
          typename Source,
          typename Function>
class map_transformation final : public transformation<T, Source, Function, map_transformation<T, Source, Function>>
{
public:

    using begin_iterator = begin_map_iterator<T, source_begin_iterator, Function>;
    using end_iterator = transform_iterator<T, source_end_iterator>;

    map_transformation(const Source& source, const Function& function) noexcept
        : transformation(source, function)
    {
    }

    begin_iterator begin() const noexcept(std::is_nothrow_constructible_v<begin_iterator, source_begin_iterator, Function>)
    {
        return begin_iterator(source.begin(), function);
    }

    end_iterator end() const noexcept(std::is_nothrow_constructible_v<end_iterator, source_end_iterator>)
    {
        return end_iterator(source.end());
    }
};

} // cppstream namespace
