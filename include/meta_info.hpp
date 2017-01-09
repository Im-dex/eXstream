#pragma once

namespace cppstream {

template <bool IsOrdered,
          bool IsDistinct,
          Order AnOrder>
struct meta_info final
{
    static constexpr bool is_ordered  = IsOrdered;
    static constexpr bool is_distinct = IsDistinct;
    static constexpr Order order      = AnOrder;
};

} // cppstream namespace
