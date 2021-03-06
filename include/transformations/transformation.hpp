#pragma once

#include "with_transformations.hpp"
#include "terminator.hpp"

namespace exstream {

template <typename T,
          typename Source,
          typename TransformIterator,
          typename Allocator,
          typename Meta,
          typename Self>
class base_transformation : public with_transformations<T, Self>,
                            public terminator<T, Self>
{
public:

    using iterator_type = TransformIterator;
    using allocator = Allocator;
    using meta = Meta;

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
          typename TransformIterator,
          typename Allocator,
          typename Meta>
class transformation : public base_transformation<T, Source, TransformIterator, Allocator, Meta, transformation<T, Source, Function, TransformIterator, Allocator, Meta>>
{
    using source_iterator = decltype(std::declval<const Source>().get_iterator());
    static_assert(std::is_constructible_v<TransformIterator, source_iterator, const Function&, const Allocator&>, "Invalid TransformIterator");
public:

    explicit transformation(const Source& source, const Function& function, const Allocator& alloc) noexcept
        : base_transformation(source, alloc),
          function(function)
    {
    }

    transformation(transformation&&) = default;

    transformation(const transformation&) = delete;
    transformation& operator= (const transformation&) = delete;

    TransformIterator get_iterator() const noexcept(std::is_nothrow_constructible_v<TransformIterator, source_iterator, const Function&> &&
                                                    std::is_nothrow_move_constructible_v<TransformIterator>)
    {
        return TransformIterator(source.get_iterator(), function, get_allocator());
    }

private:

    const Function& function;
};

template <typename T,
          typename Source,
          typename TransformIterator,
          typename Allocator,
          typename Meta>
class independent_transformation : public base_transformation<T, Source, TransformIterator, Allocator, Meta, independent_transformation<T, Source, TransformIterator, Allocator, Meta>>
{
public:

    explicit independent_transformation(const Source& source, const Allocator& alloc) noexcept
        : base_transformation(source, alloc)
    {
    }

    independent_transformation(independent_transformation&&) = default; // TODO: hide from user

    independent_transformation(const independent_transformation&) = delete;
    independent_transformation& operator= (const independent_transformation&) = delete;

    TransformIterator get_iterator() const noexcept(std::is_nothrow_constructible_v<TransformIterator, const Source&> &&
                                                    std::is_nothrow_move_constructible_v<TransformIterator>)
    {
        return TransformIterator(source.get_iterator(), get_allocator());
    }
};

} // exstream namespace
