#pragma once

#include "with_transformations.hpp"

namespace cppstream {

template <typename T,
          typename Source,
          typename Function,
          typename TransformRange>
class transformation : public with_transformations<T, transformation<T, Source, Function, TransformRange>>
{
    using source_range = decltype(std::declval<const Source>().get_range());

    static_assert(std::is_constructible_v<TransformRange, source_range, const Function&>, "Invalid TransformRange");
public:

    using range_type = TransformRange;

    explicit transformation(const Source& source, const Function& function) noexcept
        : source(source),
          function(function)
    {
    }

    transformation(transformation&&) = default; // TODO: hide from user

    transformation(const transformation&) = delete;
    transformation& operator= (const transformation&) = delete;

    TransformRange get_range() const noexcept(std::is_nothrow_constructible_v<TransformRange, source_range, const Function&> &&
                                              std::is_nothrow_move_constructible_v<TransformRange>)
    {
        return TransformRange(source.get_range(), function);
    }

private:

    const Source& source;
    const Function& function;
};

} // cppstream namespace
