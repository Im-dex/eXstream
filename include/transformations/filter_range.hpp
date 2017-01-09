#pragma once

#include "transform_range.hpp"

namespace cppstream {
namespace detail {
namespace filter {

template <typename Range, typename Function>
constexpr bool is_nothrow_synchronize()
{
    return noexcept(std::declval<Range>().at_end())  &&
           noexcept(std::declval<Range>().advance()) &&
           noexcept(std::declval<const Function&>()(std::declval<typename Range::value_type>()));
}

}} // detail::filter namespace

template <typename Range,
          typename Function,
          typename Meta>
class filter_range final : public transform_range<Range>
{
public:

    using value_type = typename Range::value_type;
    using meta = Meta;

    template <typename Allocator>
    filter_range(const Range& range, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Range>)
        : transform_range(range),
          function(function),
          synchronized(false)
    {
    }

    template <typename Allocator>
    filter_range(Range&& range, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Range>)
        : transform_range(std::move(range)),
          function(function),
          synchronized(false)
    {
    }

    filter_range(const filter_range&) = delete;
    filter_range(filter_range&&) = default;

    filter_range& operator= (const filter_range&) = delete;
    filter_range& operator= (filter_range&&) = delete;

    bool at_end() /*TODO: const*/ noexcept(detail::filter::is_nothrow_synchronize<Range, Function>())
    {
        synchronize();
        return range.at_end();
    }

    void advance() noexcept(detail::filter::is_nothrow_synchronize<Range, Function>())
    {
        range.advance();
        synchronized = false;
    }

    value_type get_value() noexcept(detail::filter::is_nothrow_synchronize<Range, Function>())
    {
        synchronize();
        return range.get_value();
    }

private:

    void synchronize() noexcept(detail::filter::is_nothrow_synchronize<Range, Function>())
    {
        if (synchronized) return;

        while (!(range.at_end() || function(range.get_value())))
        {
            range.advance();
        }

        synchronized = true;
    }

    const Function& function;
    bool synchronized;
};

} // cppstream namespace
