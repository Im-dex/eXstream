#pragma once

#include "transform_range.hpp"

namespace cppstream {

template <typename Range, typename Function>
class map_range final : public transform_range<Range>
{
public:

    using value_type = std::result_of_t<const Function&(typename Range::value_type)>;

    template <typename Allocator>
    explicit map_range(const Range& range, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : transform_range(range),
          function(function)
    {
    }

    template <typename Allocator>
    explicit map_range(Range&& range, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : transform_range(std::move(range)),
          function(function)
    {
    }

    map_range(const map_range&) = delete;
    map_range(map_range&&) = default;

    map_range& operator= (const map_range&) = delete;
    map_range& operator= (map_range&&) = delete;

    bool at_end() const noexcept(noexcept(std::declval<const Range>().at_end()))
    {
        return range.at_end();
    }

    void advance() noexcept(noexcept(std::declval<Range>().advance()))
    {
        range.advance();
    }

    // TODO: replace reference to primitive type with a value type
    value_type get_value() noexcept(noexcept(std::declval<const Function&>()(std::declval<typename Range::value_type>())))
    {
        return function(range.get_value());
    }

private:

    const Function& function;
};

} // cppstream namespace
