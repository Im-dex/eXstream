#pragma once

#include "config.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <array>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {

template <typename... Ts>
std::array<std::common_type_t<Ts...>, sizeof...(Ts)> make_array(Ts&&... vals)
{
    return { std::forward<Ts>(vals)... };
}

} // exstream namespace
