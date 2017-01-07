#pragma once

#include "mixins.hpp"

namespace cppstream {

template <typename T,
          typename Source,
          typename Function,
          typename BeginTransformIterator,
          typename EndTransformIterator>
class transformation : public with_transformations<T, transformation<T, Source, Function, BeginTransformIterator, EndTransformIterator>>
{
    using source_begin_iterator = decltype(std::declval<const Source>().begin());
    using source_end_iterator = decltype(std::declval<const Source>().end());

    static_assert(std::is_constructible_v<BeginTransformIterator, source_begin_iterator, const Function&>, "Invalid BeginTransformIterator");
    static_assert(std::is_constructible_v<EndTransformIterator, source_end_iterator>, "Invalid EndTransformIterator");
public:

    using begin_iterator = BeginTransformIterator;
    using end_iterator = EndTransformIterator;

    explicit transformation(const Source& source, const Function& function) noexcept
        : source(source),
          function(function)
    {
    }

    transformation(transformation&&) = default; // TODO: hide from user

    transformation(const transformation&) = delete;
    transformation& operator= (const transformation&) = delete;

    begin_iterator begin() const noexcept(std::is_nothrow_constructible_v<begin_iterator, source_begin_iterator, const Function&> &&
                                          std::is_nothrow_move_constructible_v<begin_iterator>)
    {
        return begin_iterator(source.begin(), function);
    }

    end_iterator end() const noexcept(std::is_nothrow_constructible_v<end_iterator, source_end_iterator> &&
                                      std::is_nothrow_move_constructible_v<end_iterator>)
    {
        return end_iterator(source.end());
    }

private:

    const Source& source;
    const Function& function;
};

} // cppstream namespace
