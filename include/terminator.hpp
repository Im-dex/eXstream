#pragma once

#include "detail/container_factory_traits.hpp"

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
    return noexcept(std::declval<const Self&>().get_iterator().elements_count());
}

template <typename Self,
          typename Container,
          typename... Args>
constexpr bool is_nothrow_to() noexcept
{
    using traits = container_factory_traits<Container>;;
    using iterator_t = typename Self::iterator_type;
    using result_type = typename iterator_t::result_type;

    return std::is_nothrow_constructible_v<Container, Args...>                               &&
           noexcept(traits::append(std::declval<Container&>(), std::declval<result_type>())) &&
           noexcept(traits::reserve(std::declval<Container&>(), size_t(0)))                  &&
           terminate::is_nothrow_elements_count<Self>()                                      &&
           terminate::is_nothrow_next<Self>();
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

    size_t count() noexcept(detail::terminate::is_nothrow_skip<Self>() &&
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

    template <typename Container, typename... Args>
    Container to(Args&&... args) noexcept(detail::terminate::is_nothrow_to<Self, Container, Args...>())
    {
        using traits = container_factory_traits<Container>;
        using condition = std::conjunction<
            std::is_constructible<Container, Args...>,
            std::negation<
                std::is_same<traits, unsupported_container_factory_traits>
            >
        >;

        return to<Container, traits>(condition(), std::forward<Args>(args)...);
    }

private:

    template <typename Container, typename Traits, typename... Args>
    Container to(std::true_type, Args&&... args) noexcept(detail::terminate::is_nothrow_to<Self, Container, Args...>())
    {
        Container container(std::forward<Args>(args)...);
        auto iter = self().get_iterator();
        const auto elements_count = iter.elements_count();

        if (elements_count != npos)
            Traits::reserve(container, elements_count);

        while (iter.has_next())
            Traits::append(container, iter.next());

        return std::move(container);
    }

    template <typename Container, typename Traits, typename... Args>
    [[noreturn]] static Container to(std::false_type, Args&&...) noexcept
    {
        static_assert(std::is_constructible_v<Container, Args...>, "Container is not constructible from the passed arguments");
        static_assert(!std::is_same_v<Traits, unsupported_container_factory_traits>, "Container isn't meet requrements"); // TODO: output requirements
        std::terminate(); // dummy terminate
    }

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }
};

} // exstream namespace
