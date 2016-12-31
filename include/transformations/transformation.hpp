#pragma once

#include "mixins.hpp"

namespace cppstream {

template <typename T,
          typename Source,
          typename Function>
class transformation : public with_map<T, Self>,
                       public with_flat_map<T, Self>
{
public:

    explicit transformation(const Source& source, const Function& function) noexcept
        : source(source),
          function(function)
    {
    }

    transformation(const transformation&) = delete;
    transformation(transformation&&) = delete;

    transformation& operator= (const transformation&) = delete;
    transformation& operator= (transformation&&) = delete;

    ~transformation() noexcept = default;

private:

    //using source_begin_iterator = decltype(std::declval<Source>().begin());
    //using source_end_iterator = decltype(std::declval<Source>().end());

    const Source& source;
    const Function& function;
};

} // cppstream namespace
