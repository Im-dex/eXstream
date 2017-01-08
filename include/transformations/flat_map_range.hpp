#pragma once

#include "transform_range.hpp"
#include "option.hpp"

namespace cppstream {

template <typename Range, typename Function>
class flat_map_range final : public transform_range<Range>
{
    using stream_t = remove_cvr_t<std::result_of_t<const Function&(typename Range::value_type)>>;
    using stream_begin_iterator = remove_cvr_t<decltype(std::begin(std::declval<stream_t>()))>;
    using stream_end_iterator = remove_cvr_t<decltype(std::end(std::declval<stream_t>()))>;
public:

    using value_type = decltype(*std::declval<stream_begin_iterator>());

    explicit flat_map_range(const Range& range, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Range> &&
                                                                                   std::is_nothrow_default_constructible_v<stream_begin_iterator>)
        : transform_range(range),
          function(function),
          streamIterator(),
          stream()
    {
    }

    explicit flat_map_range(Range&& range, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Range> &&
                                                                              std::is_nothrow_default_constructible_v<stream_begin_iterator>)
        : transform_range(std::move(range)),
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
    value_type get_value() noexcept(noexcept(std::declval<const Function&>()(std::declval<typename Range::value_type>())) &&
                                    noexcept(std::begin(std::declval<stream_t>()))                                        &&
                                    noexcept(*std::declval<stream_begin_iterator>()))
    {
        if (stream.empty())
        {
            stream = function(range.get_value());
            streamIterator = std::begin(stream.get());
        }

        return *streamIterator;
    }

private:

    const Function& function;
    stream_begin_iterator streamIterator;
    option<stream_t> stream;
};

} // cppstream namespace
