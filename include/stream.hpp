#pragma once

#include "transformations/mixins.hpp"
#include "transformations/transformation.hpp"

namespace cppstream {

template <typename T,
          typename BeginIterator,
          typename EndIterator>
class stream final : public with_transformations<T, stream<T, BeginIterator, EndIterator>>
{
public:

    using begin_iterator = BeginIterator;
    using end_iterator = EndIterator;

    explicit stream(const begin_iterator& begin_, const end_iterator& end_) noexcept(std::is_nothrow_copy_constructible_v<begin_iterator> &&
                                                                                     std::is_nothrow_copy_constructible_v<end_iterator>)
        : begin_(begin_),
          end_(end_)
    {
    }

    explicit stream(begin_iterator&& begin_, end_iterator&& end_) noexcept(std::is_nothrow_move_constructible_v<begin_iterator> &&
                                                                           std::is_nothrow_move_constructible_v<end_iterator>)
        : begin_(std::move(begin_)),
          end_(std::move(end_))
    {
    }

    stream(stream&&) = default; // TODO: hide from user

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    begin_iterator begin() const noexcept
    {
        return begin_;
    }

    end_iterator end() const noexcept
    {
        return end_;
    }

private:

    begin_iterator begin_;
    end_iterator end_;
};

} // cppstream namespace
