#pragma once

#include "transformations/mixins.hpp"

namespace cppstream {

template <typename T,
          typename BeginIter,
          typename EndIter>
class stream final : public with_map<T, stream<T, BeginIter, EndIter>>,
                     public with_flat_map<T, stream<T, BeginIter, EndIter>>
{
public:

    explicit stream(const BeginIter& begin_, const EndIter& end_) noexcept(std::is_nothrow_copy_constructible_v<BeginIter> &&
                                                                           std::is_nothrow_copy_constructible_v<EndIter>)
        : begin_(begin_),
          end_(end_)
    {
    }

    explicit stream(BeginIter&& begin_, EndIter&& end_) noexcept(std::is_nothrow_move_constructible_v<BeginIter> &&
                                                                 std::is_nothrow_move_constructible_v<EndIter>)
        : begin_(std::move(begin_)),
          end_(std::move(end_))
    {
    }

    explicit stream(const BeginIter& begin_, EndIter&& end_) noexcept(std::is_nothrow_copy_constructible_v<BeginIter> &&
                                                                      std::is_nothrow_move_constructible_v<EndIter>)
        : begin_(begin_),
          end_(std::move(end_))
    {
    }

    explicit stream(BeginIter&& begin_, const EndIter& end_) noexcept(std::is_nothrow_move_constructible_v<BeginIter> &&
                                                                      std::is_nothrow_copy_constructible_v<EndIter>)
        : begin_(std::move(begin_)),
          end_(end_)
    {
    }

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    stream(stream&&) noexcept(std::is_nothrow_move_constructible_v<BeginIter> && std::is_nothrow_move_constructible_v<EndIter>) = default;
    stream& operator= (stream&&) noexcept(std::is_nothrow_move_assignable_v<BeginIter> && std::is_nothrow_move_assignable_v<EndIter>) = default;

    BeginIter& begin() const noexcept
    {
        return begin_;
    }

    EndIter& end() const noexcept
    {
        return end_;
    }

private:

    BeginIter begin_;
    EndIter end_;
};

} // cppstream namespace
