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

template <typename Self>
constexpr bool is_nothrow_elements_count() noexcept
{
    return noexcept(std::declval<const Self&>().get_iterator().size());
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

    size_t count() const noexcept(detail::terminate::is_nothrow_skip<Self>() &&
                                  detail::terminate::is_nothrow_elements_count<Self>())
    {
        auto iter = self().get_iterator();
        const auto elementsCount = iter.elements_count();

        if (elementsCount != npos)
            return elementsCount;

        auto counter = size_t(0);
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
