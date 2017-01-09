#pragma once

#include "transform_range.hpp"
#include "option.hpp"

namespace cppstream {
namespace detail {
namespace flat_map {

template <typename Allocator, bool AllocatorUsed = false>
struct allocator_holder final
{
    explicit allocator_holder(const Allocator&) noexcept
    {
    }

    allocator_holder(allocator_holder&&) = default;

    allocator_holder(const allocator_holder&) = delete;
    allocator_holder& operator= (const allocator_holder&) = delete;
};

template <typename Allocator>
struct allocator_holder<Allocator, true> final
{
    explicit allocator_holder(const Allocator& alloc) noexcept
        : alloc(alloc)
    {
    }

    allocator_holder(allocator_holder&&) = default;

    allocator_holder(const allocator_holder&) = delete;
    allocator_holder& operator= (const allocator_holder&) = delete;

    const Allocator& alloc;
};

template <typename Function, typename Range>
constexpr bool is_nothrow_function_call() noexcept
{
    return noexcept(std::declval<const Function&>()(std::declval<typename Range::value_type>()));
}

}} // detail::flat_map namespace

template <typename Range,
          typename Function,
          typename Allocator>
class flat_map_range final : public transform_range<Range>
{
    using stream_t = remove_cvr_t<std::result_of_t<const Function&(typename Range::value_type)>>;
    using stream_begin_iterator = remove_cvr_t<decltype(std::begin(std::declval<stream_t>()))>;
    using stream_end_iterator = remove_cvr_t<decltype(std::end(std::declval<stream_t>()))>;
public:

    using value_type = decltype(*std::declval<stream_begin_iterator>());

    explicit flat_map_range(const Range& range, const Function& function, const Allocator& alloc) noexcept(std::is_nothrow_copy_constructible_v<Range> &&
                                                                                                           std::is_nothrow_default_constructible_v<stream_begin_iterator>)
        : transform_range(range),
          allocHolder(alloc),
          function(function),
          streamIterator(),
          stream()
    {
    }

    explicit flat_map_range(Range&& range, const Function& function, const Allocator& alloc) noexcept(std::is_nothrow_move_constructible_v<Range> &&
                                                                                                      std::is_nothrow_default_constructible_v<stream_begin_iterator>)
        : transform_range(std::move(range)),
          allocHolder(alloc),
          function(function),
          streamIterator(),
          stream()
    {
    }

    flat_map_range(const flat_map_range&) = delete;
    flat_map_range(flat_map_range&&) = default;

    flat_map_range& operator= (const flat_map_range&) = delete;
    flat_map_range& operator= (flat_map_range&&) = delete;

    bool at_end() const noexcept(noexcept(std::declval<const Range>().at_end()) &&
                                 noexcept(std::end(std::declval<stream_t>()))   &&
                                 is_nothrow_comparable_to_v<stream_begin_iterator, stream_end_iterator>)
    {
        return range.at_end() && (stream.empty() || (streamIterator == std::end(stream.get())));
    }

    void advance() noexcept(noexcept(std::declval<Range>().advance())         &&
                            noexcept(++std::declval<stream_begin_iterator>()) &&
                            std::is_nothrow_destructible_v<stream_t>          &&
                            is_nothrow_comparable_to_v<stream_begin_iterator, stream_end_iterator>)
    {
        if (stream.non_empty() && (streamIterator != std::end(stream.get())))
        {
            ++streamIterator;
        }
        else
        {
            stream.reset();
            range.advance();
        }
    }

    // TODO: value_type<std::remove_reference_t<T>>
    value_type get_value() noexcept(detail::flat_map::is_nothrow_function_call<Function, Range>() &&
                                    noexcept(std::begin(std::declval<stream_t>()))                &&
                                    noexcept(*std::declval<stream_begin_iterator>())) // TODO: stream_t noexcept + emplace or assign noexcept
    {
        if (stream.empty())
        {
            auto&& value = function(range.get_value());

            constexpr_if<is_allocator_used::value>()
                .then([&](auto) noexcept(noexcept(std::declval<option<stream_t>>().emplace(std::declval<decltype(value)>(), std::declval<const Allocator&>())))
                {
                    stream.emplace(std::forward<decltype(value)>(value), allocHolder.alloc);
                })
                .else_([&](auto) noexcept(std::is_nothrow_assignable_v<option<stream_t>, decltype(value)>)
                {
                    stream = std::forward<decltype(value)>(value);
                })();

            streamIterator = std::begin(stream.get());
        }

        return *streamIterator;
    }

private:

    using is_allocator_used = std::is_constructible<stream_t, stream_t&&, const Allocator&>;

    using alloc_holder = detail::flat_map::allocator_holder<Allocator, is_allocator_used::value>;

    alloc_holder allocHolder;
    const Function& function;
    stream_begin_iterator streamIterator;
    option<stream_t> stream;
};

} // cppstream namespace
