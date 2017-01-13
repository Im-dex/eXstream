#pragma once

#include "transform_iterator.hpp"
#include "option.hpp"
#include "meta_info.hpp"
#include <functional>

namespace cppstream {
namespace detail {
namespace flat_map {

template <typename Stream>
using stream_iterator_t = std::decay_t<decltype(std::begin(std::declval<Stream>()))>;

template <typename Stream>
using stream_end_iterator_t = std::decay_t<decltype(std::end(std::declval<Stream>()))>;

template <typename Stream, bool IsReference = false, bool IsReferenceToConst = false>
class stream_hold_iterator final
{
    using stream_iterator = std::move_iterator<stream_iterator_t<Stream>>;
    using stream_end_iterator = std::move_iterator<stream_end_iterator_t<Stream>>;
public:

    using value_type = typename std::iterator_traits<stream_iterator>::value_type;
    using reference = typename std::iterator_traits<stream_iterator>::reference;

    explicit stream_hold_iterator(Stream&& stream) noexcept(std::is_nothrow_move_constructible_v<Stream>                          &&
                                                            noexcept(std::make_move_iterator(std::begin(std::declval<Stream>()))) &&
                                                            noexcept(std::make_move_iterator(std::end(std::declval<Stream>()))))
        : stream(std::move(stream)),
          iter(std::make_move_iterator(std::begin(this->stream))),
          end(std::make_move_iterator(std::end(this->stream)))
    {
    }

    stream_hold_iterator(stream_hold_iterator&&) = default;

    stream_hold_iterator(const stream_hold_iterator&) = delete;
    stream_hold_iterator& operator= (const stream_hold_iterator&) = delete;

    bool at_end() const noexcept
    {
        return iter == end;
    }

    void advance() noexcept(noexcept(++std::declval<stream_iterator>()))
    {
        ++iter;
    }

    reference get_value() noexcept(noexcept(*std::declval<stream_iterator>()))
    {
        return *iter;
    }

private:

    Stream stream;
    stream_iterator iter;
    stream_end_iterator end;
};

template <typename Stream, bool IsReferenceToConst>
class stream_hold_iterator<Stream, true /* IsReference */, IsReferenceToConst> final
{
    using stream_t = std::conditional_t<
        IsReferenceToConst,
        const Stream,
        Stream
    >;

    using ref_wrap = std::reference_wrapper<stream_t>;

    using stream_iterator = stream_iterator_t<stream_t>;
    using stream_end_iterator = stream_end_iterator_t<stream_t>;
public:

    using value_type = typename std::iterator_traits<stream_iterator>::value_type;
    using reference = typename std::iterator_traits<stream_iterator>::reference;

    explicit stream_hold_iterator(stream_t& stream) noexcept(noexcept(std::begin(std::declval<stream_t&>())) &&
                                                             noexcept(std::end(std::declval<stream_t&>())))
        : streamRef(stream),
          iter(std::begin(stream())),
          end(std::end(stream()))
    {
    }

    explicit stream_hold_iterator(ref_wrap streamRef) noexcept(noexcept(std::begin(std::declval<stream_t&>())) &&
                                                               noexcept(std::end(std::declval<stream_t&>())))
        : streamRef(streamRef),
          iter(std::begin(stream())),
          end(std::end(stream()))
    {
    }

    stream_hold_iterator(stream_hold_iterator&&) = default;

    stream_hold_iterator(const stream_hold_iterator&) = delete;
    stream_hold_iterator& operator= (const stream_hold_iterator&) = delete;

    bool at_end() const noexcept
    {
        return iter == end;
    }

    void advance() noexcept(noexcept(++std::declval<stream_iterator>()))
    {
        ++iter;
    }

    reference get_value() noexcept(noexcept(*std::declval<stream_iterator>()))
    {
        return *iter;
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

template <typename T>
struct extract_stream_type final
{
    using type = T;
};

template <typename T>
struct extract_stream_type<std::reference_wrapper<T>> final
{
    using type = std::remove_cv_t<T>;
};

template <typename T>
constexpr bool is_reference_to_const_v = false;

template <typename T>
constexpr bool is_reference_to_const_v<const T&> = true;

template <typename T>
constexpr bool is_reference_to_const_v<std::reference_wrapper<const T>> = true;

template <typename Iterator,
          typename Function,
          typename StreamHoldIterator>
constexpr bool is_nothrow_fetch() noexcept
{
    return std::is_nothrow_destructible_v<StreamHoldIterator> &&
           noexcept(std::declval<Iterator>().advance())       &&
           noexcept(
               std::declval<option<StreamHoldIterator>>().emplace(
                   std::declval<const Function&>()(
                       std::declval<typename Iterator::reference>()
                   )
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
    using decayed_function_result = std::decay_t<function_result>;

    using stream_t = typename detail::flat_map::extract_stream_type<decayed_function_result>::type;

    using stream_hold_iterator = detail::flat_map::stream_hold_iterator<
        stream_t,
        std::conditional_t<
            is_reference_wrapper_v<decayed_function_result>,
            std::true_type,
            std::is_lvalue_reference<function_result>
        >::value, // is reference
        detail::flat_map::is_reference_to_const_v<decayed_function_result> // is reference to const
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

    bool at_end() noexcept(noexcept(std::declval<Iterator>().at_end()) &&
                           noexcept(std::declval<stream_hold_iterator>().at_end()))
    {
        return iterator.at_end() && (streamIterator.empty() || streamIterator.get().at_end());
    }

    void advance() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>() &&
                            noexcept(std::declval<stream_hold_iterator>().advance()))
    {
        if (streamIterator.empty() || streamIterator.get().at_end())
        {
            fetch_next_stream();
        }
        else
        {
            streamIterator.get().advance();
        }
    }

    reference get_value() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>() &&
                                   noexcept(std::declval<stream_hold_iterator>().get_value()))
    {
        if (streamIterator.empty()) fetch_next_stream();
        return streamIterator.get().get_value();
    }

private:

    void fetch_next_stream() noexcept(detail::flat_map::is_nothrow_fetch<Iterator, Function, stream_hold_iterator>())
    {
        streamIterator.emplace(function(iterator.get_value()));
        iterator.advance();
    }

    option<stream_hold_iterator> streamIterator;
    const Function& function;
};

} // cppstream namespace
