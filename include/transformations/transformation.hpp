#pragma once

#include "with_transformations.hpp"

namespace cppstream {

template <typename T,
          typename Source,
          typename TransformRange,
          typename Allocator,
          typename Self>
class base_transformation : public with_transformations<T, Self>
{
public:

    using range_type = TransformRange;
    using allocator = Allocator;

    const Allocator& get_allocator() const noexcept
    {
        return alloc;
    }

protected:

    explicit base_transformation(const Source& source, const Allocator& alloc) noexcept
        : source(source),
          alloc(alloc)
    {
    }

    base_transformation(base_transformation&&) = default;

    base_transformation(const base_transformation&) = delete;
    base_transformation& operator= (const base_transformation&) = delete;

    const Source& source;

private:

    const Allocator& alloc;
};

template <typename T,
          typename Source,
          typename Function,
          typename TransformRange,
          typename Allocator>
class transformation : public base_transformation<T, Source, TransformRange, Allocator, transformation<T, Source, Function, TransformRange, Allocator>>
{
    using source_range = decltype(std::declval<const Source>().get_range());
    static_assert(std::is_constructible_v<TransformRange, source_range, const Function&, const Allocator&>, "Invalid TransformRange");
public:

    explicit transformation(const Source& source, const Function& function, const Allocator& alloc) noexcept
        : base_transformation(source, alloc),
          function(function)
    {
    }

    transformation(transformation&&) = default; // TODO: hide from user

    transformation(const transformation&) = delete;
    transformation& operator= (const transformation&) = delete;

    TransformRange get_range() const noexcept(std::is_nothrow_constructible_v<TransformRange, source_range, const Function&> &&
                                              std::is_nothrow_move_constructible_v<TransformRange>)
    {
        return TransformRange(source.get_range(), function, get_allocator());
    }

private:

    const Function& function;
};

template <typename T,
          typename Source,
          typename TransformRange,
          typename Allocator>
class independent_transformation : public base_transformation<T, Source, TransformRange, Allocator, independent_transformation<T, Source, TransformRange, Allocator>>
{
public:

    explicit independent_transformation(const Source& source, const Allocator& alloc) noexcept
        : base_transformation(source, alloc)
    {
    }

    independent_transformation(independent_transformation&&) = default; // TODO: hide from user

    independent_transformation(const independent_transformation&) = delete;
    independent_transformation& operator= (const independent_transformation&) = delete;

    TransformRange get_range() const noexcept(std::is_nothrow_constructible_v<TransformRange, const Source&> &&
                                              std::is_nothrow_move_constructible_v<TransformRange>)
    {
        return TransformRange(source.get_range(), get_allocator());
    }
};

} // cppstream namespace
