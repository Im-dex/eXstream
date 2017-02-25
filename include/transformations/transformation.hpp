#pragma once

#include "with_transformations.hpp"
#include "terminator.hpp"

namespace exstream {

template <typename T,
          typename Source,
          typename TransformIterator,
          typename Meta,
          typename Self>
class base_transformation : public with_transformations<T, Self>,
                            public terminator<T, Self>
{
public:

    using iterator_type = TransformIterator;
    using meta = Meta;

protected:

    explicit base_transformation(const Source& source) noexcept
        : source(source)
    {
    }

    base_transformation(base_transformation&&) = default;

    base_transformation(const base_transformation&) = delete;
    base_transformation& operator= (const base_transformation&) = delete;

    const Source& source;
};

template <typename T,
          typename Source,
          typename Function,
          typename TransformIterator,
          typename Meta>
class transformation : public base_transformation<T, Source, TransformIterator, Meta, transformation<T, Source, Function, TransformIterator, Meta>>
{
    using source_iterator = decltype(std::declval<const Source>().get_iterator());
    static_assert(std::is_constructible_v<TransformIterator, source_iterator, const Function&>, "Invalid TransformIterator");
public:

    explicit transformation(const Source& source, const Function& function) noexcept
        : base_transformation(source),
          function(function)
    {
    }

    transformation(transformation&&) = default;

    transformation(const transformation&) = delete;
    transformation& operator= (const transformation&) = delete;

    TransformIterator get_iterator() const noexcept(std::is_nothrow_constructible_v<TransformIterator, source_iterator, const Function&> &&
                                                    std::is_nothrow_move_constructible_v<TransformIterator>)
    {
        return TransformIterator(source.get_iterator(), function);
    }

private:

    const Function& function;
};

template <typename T,
          typename Source,
          typename TransformIterator,
          typename Meta>
class independent_transformation : public base_transformation<T, Source, TransformIterator, Meta, independent_transformation<T, Source, TransformIterator, Meta>>
{
public:

    explicit independent_transformation(const Source& source) noexcept
        : base_transformation(source)
    {
    }

    independent_transformation(independent_transformation&&) = default; // TODO: hide from user

    independent_transformation(const independent_transformation&) = delete;
    independent_transformation& operator= (const independent_transformation&) = delete;

    TransformIterator get_iterator() const noexcept(std::is_nothrow_constructible_v<TransformIterator, const Source&> &&
                                                    std::is_nothrow_move_constructible_v<TransformIterator>)
    {
        return TransformIterator(source.get_iterator());
    }
};

} // exstream namespace
