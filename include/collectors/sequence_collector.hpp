#pragma once

#include "detail/constexpr_if.hpp"
#include "detail/traits.hpp"

namespace exstream {

template <typename T,
          typename Allocator,
          template <typename, typename> class Sequence>
class sequence_builder final
{
    using sequence_t = Sequence<T, Allocator>;
public:

    sequence_builder() = default;

    explicit sequence_builder(sequence_t&& sequence) noexcept(std::is_nothrow_move_constructible_v<sequence_t>)
        : sequence(std::move(sequence))
    {
    }

    sequence_builder(const sequence_builder&) = delete;
    sequence_builder(sequence_builder&&) = default;

    sequence_builder& operator= (const sequence_builder&) = delete;
    sequence_builder& operator= (sequence_builder&&) = delete;

    void reserve(const size_t size) noexcept(!detail::has_reserve_method_v<sequence_t, size_t>)
    {
        constexpr_if<detail::has_reserve_method_v<sequence_t, size_t>>()
            .then([&](auto)
            {
                sequence.reserve(size);
            })(nothing);
    }

    void append(const T& value)
    {
        sequence.push_back(value);
    }

    void append(T&& value)
    {
        sequence.push_back(std::move(value));
    }

    sequence_t build() noexcept(std::is_nothrow_move_constructible_v<sequence_t>)
    {
        return std::move(sequence);
    }

private:

    sequence_t sequence;
};

template <template <typename, typename> class Sequence>
struct generic_sequence_collector final
{
    generic_sequence_collector() noexcept = default;
    generic_sequence_collector(generic_sequence_collector&&) noexcept = default;

    generic_sequence_collector(const generic_sequence_collector&) = delete;
    generic_sequence_collector& operator= (const generic_sequence_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>) noexcept(std::is_nothrow_default_constructible_v<sequence_builder<T, std::allocator<T>, Sequence>> &&
                                     std::is_nothrow_move_constructible_v<sequence_builder<T, std::allocator<T>, Sequence>>)
    {
        return sequence_builder<T, std::allocator<T>, Sequence>();
    }
};

template <typename T,
          typename Allocator,
          template <typename, typename> class Sequence>
class sequence_collector final
{
    using sequence_t = Sequence<T, Allocator>;
public:

    explicit sequence_collector(sequence_t&& sequence) noexcept(std::is_nothrow_move_constructible_v<sequence_t>)
        : sequence(std::move(sequence))
    {
    }

    sequence_collector(sequence_collector&&) = default;

    sequence_collector(const sequence_collector&) = default;
    sequence_collector& operator= (const sequence_collector&) = default;

    template <typename U>
    std::enable_if_t<std::is_same_v<T, U>, sequence_builder<T, Allocator, Sequence>>
    builder(type_t<U>)
        noexcept(std::is_nothrow_constructible_v<sequence_builder<T, Allocator, Sequence>, sequence_t&&> &&
                 std::is_nothrow_move_constructible_v<sequence_builder<T, Allocator, Sequence>>)
    {
        return sequence_builder<T, Allocator, Sequence>(std::move(sequence));
    }

private:

    sequence_t sequence;
};

} // exstream namespace
