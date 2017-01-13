#pragma once

#include "detail/type_traits.hpp"

namespace exstream {
namespace detail {
namespace terminate {

template <typename Self>
constexpr bool is_nothrow_skip() noexcept
{
    return noexcept(std::declval<const Self&>().get_iterator().has_next()) &&
           noexcept(std::declval<const Self&>().get_iterator().skip());
}

template <typename Self>
constexpr bool is_nothrow_next() noexcept
{
    return noexcept(std::declval<const Self&>().get_iterator().has_next()) &&
           noexcept(std::declval<const Self&>().get_iterator().next());
}

}} // detail::terminate namespace

template <typename T, typename Self>
class terminator
{
public:

    terminator() noexcept = default;
    terminator(terminator&&) = default;

    terminator(const terminator&) = delete;
    terminator& operator= (const terminator&) = delete;

    // TODO: optimization: add size() -> option<size_t> method to the iterator
    size_t count() const noexcept(detail::terminate::is_nothrow_skip<Self>())
    {
        auto counter = size_t(0);
        auto iter = self().get_iterator();

        while (iter.has_next())
        {
            counter++;
            iter.skip();
        }

        return counter;
    }

private:

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }
};

} // exstream namespace
