#pragma once

#include "detail/traits.hpp"

namespace exstream {
namespace detail {

EXSTREAM_DEFINE_HAS_METHOD(reserve)
EXSTREAM_DEFINE_HAS_METHOD(push_back)
EXSTREAM_DEFINE_HAS_METHOD(insert)

template <typename Container>
constexpr bool is_nothrow_reserve() noexcept
{
    return is_nothrow_reserve<Container>(has_reserve_method<Container, size_t>());
}

template <typename Container>
constexpr bool is_nothrow_reserve(std::true_type /*has reserve(size_t) method*/) noexcept
{
    return noexcept(std::declval<Container>().reserve(std::declval<size_t>()));
}

template <typename Container>
constexpr bool is_nothrow_reserve(std::false_type /*has reserve(size_t) method*/) noexcept
{
    return true;
}

template <typename Container>
class base_container_factory_traits
{
public:

    using value_type = typename Container::value_type;

    static void reserve(Container& container, const size_t size) noexcept(is_nothrow_reserve<Container>())
    {
        reserve(container, size, has_reserve_method<Container, size_t>());
    }

private:

    static void reserve(Container&, const size_t, std::false_type /*has reserve(size_t) method*/) noexcept
    {
    }

    static void reserve(Container& container, const size_t size, std::true_type /*has reserve(size_t) method*/)
        noexcept(noexcept(container.reserve(size)))
    {
        container.reserve(size);
    }
};

} // detail namespace

struct unsupported_container_factory_traits final
{
    template <typename Container>
    static void reserve(Container&, const size_t) noexcept
    {
    }

    template <typename Container, typename T>
    static void append(Container&, T&&) noexcept
    {
    }
};

template <typename Container>
struct back_insert_aware_container_factory_traits final : detail::base_container_factory_traits<Container>
{
    static void append(Container& container, const value_type& element) noexcept(noexcept(container.push_back(element)))
    {
        container.push_back(element);
    }

    static void append(Container& container, value_type&& element) noexcept(noexcept(container.push_back(std::move(element))))
    {
        container.push_back(std::move(element));
    }
};

template <typename Container>
struct insert_aware_container_factory_traits final : detail::base_container_factory_traits<Container>
{
    static void append(Container& container, const value_type& element) noexcept(noexcept(container.insert(element)))
    {
        container.insert(element);
    }

    static void append(Container& container, value_type&& element) noexcept(noexcept(container.insert(std::move(element))))
    {
        container.insert(std::move(element));
    }
};

namespace detail {

template <typename Container, bool HasValueType = false>
struct container_factory_traits_selector final
{
    using type = unsupported_container_factory_traits;
};

template <typename Container>
struct container_factory_traits_selector<Container, /* HasValueType = */true> final
{
    using value_type = typename Container::value_type;

    using type = std::conditional_t<
        has_push_back_method_v<Container, const value_type&> && has_push_back_method_v<Container, value_type&&>,
        back_insert_aware_container_factory_traits<Container>,
        std::conditional_t<
            has_insert_method_v<Container, const value_type&> && has_insert_method_v<Container, value_type&&>,
            insert_aware_container_factory_traits<Container>,
            unsupported_container_factory_traits
        >
    >;
};

} // detail namespace

template <typename Container>
using container_factory_traits = typename detail::container_factory_traits_selector<
    Container,
    detail::has_value_type_member_v<Container>
>::type;

} // exstream namespace
