#pragma once

#include "detail/type_traits.hpp"

namespace stream {

template <typename T, bool Const>
class STREAM_NO_VTABLE iterator
{
public:

    using value_type = T;

    virtual bool is_empty() const = 0;

    virtual bool non_empty() const = 0;

    virtual std::conditional_t<Const, const T&, T&> value() = 0;

    virtual const T& value() const = 0;

    virtual void advance() = 0;
};

} // stream namespace
