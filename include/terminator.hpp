#pragma once

#include "detail/bool_c.hpp"
#include "utility.hpp"

namespace exstream {
namespace detail {
namespace terminate {

static constexpr int suppress_unnecessary_error = 0;

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
    return noexcept(std::declval<const Self&>().get_iterator().elements_count());
}

}} // detail::terminate namespace

template <typename T, typename Self>
class terminator
{
public:

    terminator() = default;
    terminator(terminator&&) = default;

    terminator(const terminator&) = default;
    terminator& operator= (const terminator&) = default;

    size_t count() noexcept(detail::terminate::is_nothrow_skip<Self>() &&
                            detail::terminate::is_nothrow_elements_count<Self>())
    {
        auto iter = self().get_iterator();
        const auto elementsCount = iter.elements_count();

        if (elementsCount != unknown_count)
            return elementsCount;

        size_t counter = 0;
        while (iter.has_next())
        {
            ++counter;
            iter.skip();
        }

        return counter;
    }

    template <typename OutputIter>
    void fill(OutputIter&& outIter)
    {
        fill(std::forward<OutputIter>(outIter), is_output_iterator<std::decay_t<OutputIter>>());
    }

    template <typename Collector>
    decltype(auto) collect(Collector&& collector)
    {
        return collect(std::forward<Collector>(collector), is_collector<Collector, T>());
    }

private:

    template <typename OutputIter>
    void fill(OutputIter outIter, std::true_type /* is output iterator */)
    {
        auto iter = self().get_iterator();
        while (!iter.has_next())
        {
            *outIter = iter.next();
            ++outIter;
        }
    }

    template <typename OutputIter>
    void fill(OutputIter, std::false_type /* is output iterator */) noexcept
    {
        static_assert(false_v<OutputIter>, "Output iterator expected");
    }

    template <typename Collector>
    decltype(auto) collect(Collector&& collector, std::true_type /* is valid collector */)
    {
        auto builder = collector.builder(type_t<T>());
        auto iter = self().get_iterator();

        const auto elementsCount = iter.elements_count();
        if (elementsCount != unknown_count)
            builder.reserve(elementsCount);

        while (iter.has_next())
            builder.append(iter.next());

        return builder.build();
    }

    template <typename Collector>
    int collect(Collector&&, std::false_type /* is valid collector */) noexcept
    {
        static_assert(false_v<Collector>, "Invalid collector");
        return detail::terminate::suppress_unnecessary_error;
    }

protected:

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }
};

} // exstream namespace
