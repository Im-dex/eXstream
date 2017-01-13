#pragma once

#include "transform_iterator.hpp"
#include "option.hpp"
#include "meta_info.hpp"

namespace exstream {
namespace detail {
namespace flat_map {

template <typename Stream>
using stream_iterator_t = std::decay_t<decltype(std::begin(std::declval<Stream>()))>;

template <typename Stream>
using stream_end_iterator_t = std::decay_t<decltype(std::end(std::declval<Stream>()))>;

template <typename Stream, bool IsReference = false>
class stream_hold_iterator final
{
    using stream_iterator = std::move_iterator<stream_iterator_t<Stream>>;
    using stream_end_iterator = std::move_iterator<stream_end_iterator_t<Stream>>;
public:

    using value_type = typename std::iterator_traits<stream_iterator>::value_type;
    using reference = typename std::iterator_traits<stream_iterator>::reference;

    explicit stream_hold_iterator(Stream&& streamArg) noexcept(std::is_nothrow_move_constructible_v<Stream>                          &&
                                                               noexcept(std::make_move_iterator(std::begin(std::declval<Stream>()))) &&
                                                               noexcept(std::make_move_iterator(std::end(std::declval<Stream>()))))
        : stream(std::move(streamArg)),
          iter(std::make_move_iterator(std::begin(stream))),
          end(std::make_move_iterator(std::end(stream)))
    {
    }

    stream_hold_iterator(stream_hold_iterator&&) = default;

    stream_hold_iterator(const stream_hold_iterator&) = delete;
    stream_hold_iterator& operator= (const stream_hold_iterator&) = delete;

    bool has_next() const noexcept(is_nothrow_comparable_to_v<const stream_iterator, const stream_end_iterator>)
    {
        return iter != end;
    }

    reference next() noexcept(noexcept(*(std::declval<stream_iterator>()++)))
    {
        return *(iter++);
    }

    void skip() noexcept(noexcept(++std::declval<stream_iterator>()))
    {
        ++iter;
    }

private:

    Stream stream;
    stream_iterator iter;
    stream_end_iterator end;
};

template <typename Stream>
class stream_hold_iterator<Stream, true /* IsReference */> final
{
    using stream_t = const Stream;
    using ref_wrap = std::reference_wrapper<stream_t>;

    using stream_iterator = stream_iterator_t<stream_t>;
    using stream_end_iterator = stream_end_iterator_t<stream_t>;
public:

    // TODO: infer??
    using value_type = typename std::iterator_traits<stream_iterator>::value_type;
    using reference = typename std::iterator_traits<stream_iterator>::reference;

    explicit stream_hold_iterator(const stream_t& stream) noexcept(noexcept(std::begin(std::declval<stream_t&>())) &&
                                                                   noexcept(std::end(std::declval<stream_t&>())))
        : streamRef(stream),
          iter(std::begin(stream())),
          end(std::end(stream()))
    {
    }

    explicit stream_hold_iterator(const ref_wrap streamRef) noexcept(noexcept(std::begin(std::declval<stream_t&>())) &&
                                                                     noexcept(std::end(std::declval<stream_t&>())))
        : streamRef(streamRef.get()),
          iter(std::begin(stream())),
          end(std::end(stream()))
    {
    }

    stream_hold_iterator(stream_hold_iterator&&) = default;

    stream_hold_iterator(const stream_hold_iterator&) = delete;
    stream_hold_iterator& operator= (const stream_hold_iterator&) = delete;

    bool has_next() const noexcept(is_nothrow_comparable_to_v<const stream_iterator, const stream_end_iterator>)
    {
        return iter != end;
    }

    reference next() noexcept(noexcept(*(std::declval<stream_iterator&>()++)))
    {
        return *(iter++);
    }

    void skip() noexcept(noexcept(++std::declval<stream_iterator&>()))
    {
        ++iter;
    }

private:

    stream_t& stream() noexcept
    {
        return streamRef.get();
    }

    ref_wrap streamRef;
    stream_iterator iter;
    stream_end_iterator end;
};

template <typename Iterator,
          typename Function,
          typename StreamHoldIterator>
constexpr bool is_nothrow_fetch() noexcept
{
    return noexcept(
        std::declval<option<StreamHoldIterator>&>().emplace(
            std::declval<const Function&>()(std::declval<Iterator&>().next())
        )
    );
}

}} // detail::flat_map namespace

template <typename Iterator,
          typename Function,
          typename Meta,
          typename Allocator>
class flat_map_iterator final : public transform_iterator<Iterator>
{
    using function_result = std::result_of_t<const Function&(typename Iterator::reference)>;
    using stream_t = typename result_traits<function_result>::type;

    using stream_hold_iterator = detail::flat_map::stream_hold_iterator<
        stream_t,
        std::disjunction_v<
            is_reference_wrapper<std::decay_t<function_result>>,
            std::is_lvalue_reference<function_result>
        >
    >;
public:

    using value_type = typename stream_hold_iterator::value_type;
    using reference = typename stream_hold_iterator::reference;
    using meta = meta_info<false, false, Order::Unknown>;

    explicit flat_map_iterator(const Iterator& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          streamIterator(),
          function(function)
    {
    }

    explicit flat_map_iterator(Iterator&& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          streamIterator(),
          function(function)
    {
    }

    flat_map_iterator(const flat_map_iterator&) = delete;
    flat_map_iterator(flat_map_iterator&&) = default;

    flat_map_iterator& operator= (const flat_map_iterator&) = delete;

    bool has_next() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>())
    {
        if (streamIterator.empty()) fetch();
        return iterator.has_next() || (streamIterator.non_empty() && streamIterator.get().has_next());
    }

    reference next() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>())
    {
        if (streamIterator.empty()) fetch();
        return streamIterator.get().next();
    }

    void skip() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>())
    {
        fetch();
    }

private:

    void fetch() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>())
    {
        streamIterator.emplace(function(iterator.next()));
    }

    option<stream_hold_iterator> streamIterator;
    const Function& function;
};

} // exstream namespace
